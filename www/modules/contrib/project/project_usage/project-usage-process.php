#!/usr/bin/php
<?php
/**
 * @file
 * Processes the project_usage statistics.
 */

// Define the root directory of the Backdrop installation.
$options = getopt('', array('url:', 'root:'));
if (!empty($options['root'])) {
  define('BACKDROP_ROOT', $options['root']);
}
else {
  $cwd = getcwd();
  if (($pos = strpos($cwd, '/sites/')) || ($pos = strpos($cwd, '/modules/'))) {
    $cwd = substr($cwd, 0, $pos);
  }
  define('BACKDROP_ROOT', $cwd);
}

// The hostname of your site. Required so that when we bootstrap Backdrop in
// this script, we find the right settings.php file in your sites folder.
define('HTTP_HOST', !empty($options['url']) ? parse_url($options['url'], PHP_URL_HOST) : '');

// ------------------------------------------------------------
// Initialization
// (Real work begins here, nothing else to customize)
// ------------------------------------------------------------

// Check if all required variables are defined
$vars = array(
  'BACKDROP_ROOT' => BACKDROP_ROOT,
  'HTTP_HOST' => HTTP_HOST,
);
$fatal_err = FALSE;
foreach ($vars as $name => $val) {
  if (empty($val)) {
    print "ERROR: \"$name\" constant not defined, aborting\n";
    $fatal_err = TRUE;
  }
}
if ($fatal_err) {
  exit(1);
}

$script_name = $argv[0];

// Setup variables for Backdrop bootstrap
$_SERVER['HTTP_HOST'] = HTTP_HOST;
$_SERVER['REMOTE_ADDR'] = '127.0.0.1';
$_SERVER['REQUEST_URI'] = '/' . $script_name;
$_SERVER['SCRIPT_NAME'] = '/' . $script_name;
$_SERVER['PHP_SELF'] = '/' . $script_name;
$_SERVER['SCRIPT_FILENAME'] = $_SERVER['PWD'] .'/'. $script_name;
$_SERVER['PATH_TRANSLATED'] = $_SERVER['SCRIPT_FILENAME'];

if (!chdir(BACKDROP_ROOT)) {
  print "ERROR: Can't chdir(BACKDROP_ROOT), aborting.\n";
  exit(1);
}

require_once BACKDROP_ROOT . '/core/includes/bootstrap.inc';
backdrop_bootstrap(BACKDROP_BOOTSTRAP_FULL);

if (!module_exists('project_usage')) {
  print "ERROR: Project usage module does not exist, aborting.\n";
  exit(1);
}

// Load the API functions we need for manipulating dates and timestamps.
module_load_include('inc', 'project_usage', 'project_usage.date');

// ------------------------------------------------------------
// Call the daily and weekly processing tasks as needed.
// ------------------------------------------------------------

$tables_updated = FALSE;

// Get the oldest data still in the raw table. If it has been more than a day
// since the oldest data has been processed, start with from that day and
// calculate that day's totals. Continue processing each day until we reach the
// current day. Data for the current (partial) day is not calculated.
$last_processed_daily_timestamp = state_get('project_usage_last_daily', REQUEST_TIME - PROJECT_USAGE_YEAR);
$start_of_current_day = project_usage_daily_timestamp();

// Run if we haven't processed yesterday's numbers.
if ($last_processed_daily_timestamp < $start_of_current_day) {
  // Assign project NIDs to the raw data since the last run.
  project_usage_process_raw_data();

  // Timestamp for beginning of the oldest available data.
  $oldest_time = (int) db_query("SELECT MIN(timestamp) FROM {project_usage_raw}")->fetchField();
  $oldest_day_end = project_usage_daily_timestamp($oldest_time, 1);
  $daily_timestamp = project_usage_daily_timestamp($oldest_time);

  // Process all days up until the current one.
  while ($daily_timestamp < $start_of_current_day) {
    project_usage_process_daily($daily_timestamp);
    // Increment the timestamp to the next day.
    $daily_timestamp = project_usage_daily_timestamp($daily_timestamp, 1);
  }

  state_set('project_usage_last_daily', $daily_timestamp);
  $tables_updated = TRUE;
}

// Process the weekly data up until the current week. Data for the current
// (partial) week is not calculated.
$last_processed_weekly_timestamp = state_get('project_usage_last_weekly', REQUEST_TIME - PROJECT_USAGE_YEAR);
$start_of_current_week = project_usage_weekly_timestamp();
if ($last_processed_weekly_timestamp < $start_of_current_week) {
  // Start with the week following last processed one.
  $next_weekly_timestamp = project_usage_weekly_timestamp($last_processed_weekly_timestamp, 1);

  // Process all weeks up until the current one.
  while ($next_weekly_timestamp < $start_of_current_week) {
    // Increment the timestamp to the next week.
    $last_weekly_timestamp = $next_weekly_timestamp;
    $next_weekly_timestamp = project_usage_weekly_timestamp($last_weekly_timestamp, 1);
    project_usage_process_weekly($last_weekly_timestamp);
    state_set('project_usage_last_weekly', $last_weekly_timestamp);
  }

  // Reset the list of active weeks.
  project_usage_get_active_weeks(TRUE);
  $tables_updated = TRUE;
}

// Wipe the cache of all expired usage pages.
if ($tables_updated) {
  project_usage_remove_expired_data();
  cache('project_usage')->flush();
}

// All done.
exit();


/**
 * Assign node IDs to all the raw data that has been written.
 */
function project_usage_process_raw_data() {
  $num_updates = 0;
  $result = db_query("SELECT DISTINCT name, version FROM {project_usage_raw} WHERE project_nid = 0 OR release_nid = 0");
  foreach ($result as $row) {
    $project_nid = db_query("SELECT nid FROM {project} WHERE name = :name", array(':name' => $row->name))->fetchField();
    if ($project_nid) {
      $release_nid = db_query("SELECT nid FROM {project_release} WHERE project_nid = :project_nid AND version = :version", array(':project_nid' => $project_nid, ':version' => $row->version))->fetchField();
      if ($release_nid) {
        $update_result = db_query("UPDATE {project_usage_raw} SET project_nid = :project_nid, release_nid = :release_nid WHERE name = :name AND version = :version", array(':project_nid' => $project_nid, ':release_nid' => $release_nid, ':name' => $row->name, ':version' => $row->version));
        $num_updates += $update_result->rowCount();
      }
    }
  }

  $substitutions = array(
    '!rows' => format_plural($num_updates, '1 row', '@count rows'),
  );

  watchdog('project_usage', 'Assigned project and release node IDs to !rows.', $substitutions);
}

/**
 * Process all the raw data for a given day.
 *
 * The primary key on the {project_usage_raw} table will prevent duplicate
 * records provided we process them once the day is complete. If we pull them
 * out too soon and the site checks in again they will be counted twice.
 *
 * @param $day_start_timestamp
 *   Timestamp indicating the start of the day for which stats should be
 *   generated.
 */
function project_usage_process_daily($day_start_timestamp) {
  // Ensure the timestamp is a starting timestamp.
  $start = project_usage_daily_timestamp($day_start_timestamp);
  $end = project_usage_daily_timestamp($day_start_timestamp, 1);

  // Safty check to prevent processing of the current day's numbers, as the
  // day has not yet finished.
  if ($end > REQUEST_TIME) {
    return;
  }

  // Move usage records with project node IDs into the daily table and remove
  // the rest.
  $result = db_query("INSERT INTO {project_usage_day} (timestamp, site_key, project_nid, release_nid, version_api, hostname) (SELECT :start, site_key, project_nid, release_nid, version_api, hostname FROM {project_usage_raw} WHERE timestamp >= :start AND timestamp < :end AND project_nid <> 0 GROUP BY site_key, project_nid)", array(':start' => $start, ':end' => $end));
  $num_new_day_rows = $result->rowCount();

  $result = db_query("DELETE FROM {project_usage_raw} WHERE timestamp >= :start and timestamp < :end", array(':start' => $start, ':end' => $end));
  $num_deleted_raw_rows = $result->rowCount();

  $substitutions = array(
    '!day_rows' => format_plural($num_new_day_rows, '1 row', '@count rows'),
    '!raw_rows' => format_plural($num_deleted_raw_rows, '1 row', '@count rows'),
    '!date' => format_date($start, 'custom', 'Y-m-d'),
  );
  watchdog('project_usage', 'Moved !day_rows from raw to daily and deleted !raw_rows of raw data for the day of !date.', $substitutions);
}

/**
 * Compute the weekly summaries for the week starting at the given timestamp.
 *
 * @param $week_start_timestamp
 *   Timestamp indicating the start of the week for which stats should be
 *   calculated.
 */
function project_usage_process_weekly($week_start_timestamp) {
  // Ensure the timestamp is a starting timestamp.
  $start = project_usage_weekly_timestamp($week_start_timestamp);
  $end = project_usage_weekly_timestamp($week_start_timestamp, 1);

  // Safety check to prevent processing of the current week's numbers, as the
  // week has not yet finished.
  if ($end > REQUEST_TIME) {
    return;
  }

  $start_date = format_date($start, 'custom', 'Y-m-d');
  $end_date = format_date(project_usage_daily_timestamp($end, -1), 'custom', 'Y-m-d');

  // Try to compute the usage tallies per project and per release. If there
  // is a problem--perhaps some rows existed from a previous, incomplete
  // run that are preventing inserts, throw a watchdog error.
  try {
    $sql = "INSERT INTO {project_usage_week_project} (nid, timestamp, version_api, count) SELECT project_nid as nid, :start, version_api, COUNT(DISTINCT site_key) FROM {project_usage_day} WHERE timestamp >= :start AND timestamp < :end AND project_nid <> 0 GROUP BY project_nid, version_api";
    $query_args = array(':start' => $start, ':end' => $end);
    $result = db_query($sql, $query_args);
    $project_count = $result->rowCount();
  }
  catch (PDOException $e) {
    $project_count = 0;
    $substitutions = array(
      '@start' => $start_date,
      '@end' => $end_date,
    );
    watchdog('project_usage', 'Weekly project tallies for the week of @start through @end already calculated. No data updated.', $substitutions);
  }

  try {
    $sql = "INSERT INTO {project_usage_week_release} (nid, timestamp, count) SELECT release_nid as nid, :start, COUNT(DISTINCT site_key) FROM {project_usage_day} WHERE timestamp >= :start AND timestamp < :end AND release_nid <> 0 GROUP BY release_nid";
    $query_args = array(':start' => $start, ':end' => $end);
    $result = db_query($sql, $query_args);
    $release_count = $result->rowCount();
  }
  catch (PDOException $e) {
    $release_count = 0;
    $substitutions = array(
      '@start' => $start_date,
      '@end' => $end_date,
    );
    watchdog('project_usage', 'Weekly release tallies for the week of @start through @end already calculated. No data updated.', $substitutions);
  }

  $substitutions = array(
    '@projects' => format_plural($project_count, '1 project', '@count projects'),
    '@releases' => format_plural($release_count, '1 release', '@count releases'),
    '@start' => $start_date,
    '@end' => $end_date,
  );
  watchdog('project_usage', 'Completed weekly usage data processing on @projects and @releases for the week of @start through @end.', $substitutions);
}

/**
 * Clean up data that is older than the configured maximum lifetimes.
 */
function project_usage_remove_expired_data() {
  // Remove old daily records.
  $daily_life = config_get('project_usage.settings', 'life_daily');
  $daily_life = $daily_life ? $daily_life : 2419200; // 28 days.
  db_query("DELETE FROM {project_usage_day} WHERE timestamp < :timestamp", array(':timestamp' => REQUEST_TIME - $daily_life));

  // Remove old weekly project records.
  $project_life = config_get('project_usage.settings', 'life_weekly_project');
  $project_life = $project_life ? $project_life : PROJECT_USAGE_YEAR;
  db_query("DELETE FROM {project_usage_week_project} WHERE timestamp < :timestamp", array(':timestamp' => REQUEST_TIME - $project_life));

  // Remove old weekly project release records.
  $release_life = config_get('project_usage.settings', 'life_weekly_release');
  $release_life = $release_life ? $release_life : PROJECT_USAGE_YEAR;
  db_query("DELETE FROM {project_usage_week_release} WHERE timestamp < :timestamp", array(':timestamp' => REQUEST_TIME - $release_life));
}
