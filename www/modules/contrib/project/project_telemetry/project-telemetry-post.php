<?php
/**
 * @file
 * Lightweight script for saving usage data from sites running Telemetry module.
 *
 * This script encourages a local .htaccess file with the following:
 *
 * @code
 * <IfModule mod_rewrite.c>
 *   RewriteEngine on
 *   RewriteRule ^telemetry/post$ modules/project/project_telemetry/project-telemetry-post.php [L,QSA]
 * </IfModule>
 * @endcode
 *
 * Or if using Nginx, add the following to your nginx site configuration file:
 *
 * @code
 *   location = /telemetry/post {
 *     rewrite ^/(.*)$ /modules/project/project_telemetry/project-telemetry-post.php;
 *   }
 * @endcode
 *
 * Configuration within this file is usually unnecessary and settings should be
 * automatically determined. If manual setting of the BACKDROP_ROOT constant is
 * required, they may be overridden in the $_ENV variable by your web server.
 */

/**
 * Determine the root of the Backdrop installation.
 */
if (isset($_ENV['BACKDROP_ROOT'])) {
  $backdrop_root = $_ENV['BACKDROP_ROOT'];
}
else {
  $cwd = getcwd();
  if (($pos = strpos($cwd, '/sites/')) || ($pos = strpos($cwd, '/modules/'))) {
    $cwd = substr($cwd, 0, $pos);
  }
  $backdrop_root = $cwd;
}
define('BACKDROP_ROOT', $backdrop_root);

// Collect JSON from the POST request.
$json = file_get_contents('php://input');
$telemetry_data = json_decode($json, TRUE);

// Basic error checking.
$error_output = NULL;
if ($telemetry_data === NULL) {
  $error_output = array(
    'error' => 'POST request did not contain valid JSON.',
  );
}
elseif (!isset($_SERVER['HTTP_X_SITE_KEY'])) {
  $error_output = array(
    'error' => 'No X-Site-Key header specified, no data saved.',
  );
}
if ($error_output) {
  header('Content-Type', 'application/json');
  echo json_encode($error_output, JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES);
  exit();
}

/**
 * Bootstrap Backdrop to database level.
 */
chdir(BACKDROP_ROOT);
include_once './core/includes/bootstrap.inc';
backdrop_bootstrap(BACKDROP_BOOTSTRAP_DATABASE);

// We can't call module_exists without bootstrapping to a higher level so
// we'll settle for checking that the table exists.
if (db_table_exists('project_telemetry_raw')) {
  $site_key = $_SERVER['HTTP_X_SITE_KEY'];
  $ip_address = ip_address();

  foreach ($telemetry_data as $project_name => $project_data) {
    // Get any version information if specified.
    $project_version = NULL;
    if (isset($project_data['version'])) {
      $project_version = $project_data['version'];
      unset($project_data['version']);
    }
    _project_telemetry_save($site_key, $project_name, $project_version, $project_data);
  }
}

/**
 * Save raw Telemetry data for a single project's data.
 *
 * @param string $site_key
 *   The unique ID for a site, generated on installation of Telemetry module.
 * @param $project_name
 *   The machine name of a project.
 * @param $project_version
 *   The version string of the project.
 * @param array $project_data
 *   The complete set of Telemetry data for this project, in a key => value
 *   array, with each key and value being a string.
 */
function _project_telemetry_save($site_key, $project_name, $project_version, array $project_data) {
  static $project_info = array();

  if (!isset($project_info[$project_name . '-' . $project_version])) {
    $project_nid = db_query('SELECT nid FROM {project} WHERE name = :project_name', [
      ':project_name' => $project_name,
    ])->fetchField();

    if (db_table_exists('project_release')) {
      $release_nid = db_query('SELECT nid FROM {project_release} WHERE project_nid = :project_nid AND version = :version', [
        ':project_nid' => $project_nid,
        ':version' => $project_version,
      ])->fetchField();
    }
    else {
      $release_nid = NULL;
    }

    $telemetry_settings = db_query('SELECT * FROM {project_telemetry} WHERE project_nid = :project_nid', [
      ':project_nid' => $project_nid,
    ])->fetchAssoc();
    $telemetry_settings['allowed_values'] = unserialize($telemetry_settings['allowed_values']);

    // Statically cache all values.
    $project_info[$project_name . '-' . $project_version] = array(
      'project_nid' => $project_nid,
      'release_nid' => $release_nid,
      'telemetry_settings' => $telemetry_settings,
    );
  }
  else {
    // Used cache data if we've already checked this project's settings.
    $cached_info = $project_info[$project_name . '-' . $project_version];
    $project_nid = $cached_info['project_nid'];
    $release_nid = $cached_info['release_nid'];
    $telemetry_settings = $cached_info['telemetry_settings'];
  }

  // Do not save values if the project does not have Telemetry data saving
  // enabled. Can be configured per project at /node/*/telemetry/settings.
  if (!$telemetry_settings['enabled']) {
    return;
  }

  // Delete previous entries.
  db_delete('project_telemetry_raw')
    ->condition('project_name', $project_name)
    ->condition('site_key', $site_key)
    ->execute();

  // Use multi-value insert to save all the new values.
  $query = db_insert('project_telemetry_raw')
    ->fields(array('project_nid', 'project_name', 'site_key', 'timestamp', 'version', 'item_key', 'item_value'));
  foreach ($project_data as $key => $value) {
    // Only save the allowed values.
    if (!array_key_exists($key, $telemetry_settings['allowed_values'])) {
      continue;
    }

    $query->values(array(
      'project_name' => $project_name,
      'site_key' => $site_key,
      'timestamp' => REQUEST_TIME,
      //'version_api' => $project_api_version,
      'version' => $project_version,
      'project_nid' => $project_nid,
      'release_nid' => $release_nid,
      'item_key' => $key,
      'item_value' => $value,
    ));
  }
  $query->execute();
}
