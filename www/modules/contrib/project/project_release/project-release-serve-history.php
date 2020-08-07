<?php
/**
 * @file
 * Ultra-thin PHP wrapper to serve XML release history files to update.module.
 *
 * This script encourages a local .htaccess file with the following:
 *
 * @code
 * DirectoryIndex project-release-serve-history.php
 * <IfModule mod_rewrite.c>
 *   RewriteEngine on
 *   RewriteRule ^release-history/(.*)$ modules/project/project_release/project-release-serve-history.php?q=$1 [L,QSA]
 * </IfModule>
 * @endcode
 *
 * Or if using Nginx, add the following to your nginx site configuration file:
 *
 * @code
 *   location ^= /release-history {
 *     rewrite ^/(.*)$ /modules/project/project_release/project-release-serve-history.php?q=$1;
 *   }
 *
 * @endcode
 *
 *
 * Configuration within this file is usually unnecessary and settings should be
 * automatically determined. If manual setting of the BACKDROP_ROOT or
 * HISTORY_ROOT constants is required, they may be overridden in the $_ENV
 * variable by your web server.
 *
 * @author Derek Wright (http://drupal.org/user/46549)
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

/**
 * Determine directory tree for the XML history files.
 */
if (isset($_ENV['HISTORY_ROOT'])) {
  $history_root = $_ENV['HISTORY_ROOT'];
}
elseif (isset($cwd)) {
  $history_root = $cwd . '/files/release-history';
}
else {
  $history_root = '';
}
define('HISTORY_ROOT', $history_root);

/**
 * Bootstrap Backdrop to database level.
 */
chdir(BACKDROP_ROOT);
include_once './core/includes/bootstrap.inc';
backdrop_bootstrap(BACKDROP_BOOTSTRAP_DATABASE);

/**
 * Find and serve the proper history file.
 */

// Set page headers for the XML response.
header('Content-Type: text/xml; charset=utf-8');

// Make sure we have the path arguments we need.
$path = isset($_GET['q']) ? $_GET['q'] : '';
$args = explode('/', trim($path, '/'));
if (empty($args[0])) {
  error('You must specify a project name of which to display the release history.');
}
else {
  $project_name = $args[0];
}
if (empty($args[1])) {
  error('You must specify an API compatibility version as the final argument to the path.');
}
else {
  $version_api = $args[1];
}

// Sanitize the user-supplied input for use in filenames.
$whitelist_regexp = '@[^a-zA-Z0-9_.-]@';
$safe_project_name = preg_replace($whitelist_regexp, '#', $project_name);
$safe_api_vers = preg_replace($whitelist_regexp, '#', $version_api);

// Figure out the filename for the release history we want to serve.
$project_dir = HISTORY_ROOT . '/' . $safe_project_name;
$filename = $safe_project_name . '-' . $safe_api_vers .'.xml';
$full_path = $project_dir . '/' . $filename;

if (!is_file($full_path)) {
  if (!is_dir($project_dir)) {
    error(strtr('No release history was found for the requested project (@project).', array('@project' => _check_plain($project_name))));
  }
  error(strtr('No release history available for @project @version.', array('@project' => _check_plain($project_name), '@version' => _check_plain($version_api))));
  exit(1);
}

// Set headers to disable caching.
$stat = stat($full_path);
$mtime = $stat[9];
header('Last-Modified: '. gmdate('D, d M Y H:i:s', $mtime) .' GMT');
header("Expires: Sun, 19 Nov 1978 05:00:00 GMT");
header("Cache-Control: store, no-cache, must-revalidate");
header("Cache-Control: post-check=0, pre-check=0", FALSE);

// Serve the contents.
$file = file_get_contents($full_path);
// Old release xml files are missing the encoding. Prepend one if necessary.
if (substr($file, 0, 5) != '<?xml') {
  echo '<?xml version="1.0" encoding="utf-8"?>' ."\n";
}
echo $file;

// Record usage statistics.
if (isset($_GET['site_key'])) {
  // We can't call module_exists without bootstrapping to a higher level so
  // we'll settle for checking that the table exists.
  if (db_table_exists('project_usage_raw')) {
    $site_key = $_GET['site_key'];
    $project_version = isset($_GET['version']) ? $_GET['version'] : '';
    $ip_address = ip_address();

    // Compute a GMT timestamp for beginning of the day. getdate() is
    // affected by the server's timezone so we need to cancel it out.
    $now = time();
    $time_parts = getdate($now - date('Z', $now));
    $timestamp = gmmktime(0, 0, 0, $time_parts['mon'], $time_parts['mday'], $time_parts['year']);

    $result = db_query("UPDATE {project_usage_raw} SET version_api = :version_api, version = :version, hostname = :hostname WHERE name = :name AND timestamp = :timestamp AND site_key = :site_key", array(':version_api' => $version_api, ':version' => $project_version, ':hostname' => $ip_address, ':name' => $project_name, ':timestamp' => $timestamp, ':site_key' => $site_key));
    if ($result->rowCount() === 0) {
      db_query("INSERT INTO {project_usage_raw} (name, timestamp, site_key, version_api, version, hostname) VALUES (:name, :timestamp, :site_key, :version_api, :version, :hostname)", array(':name' => $project_name, ':timestamp' => $timestamp, ':site_key' => $site_key, ':version_api' => $version_api, ':version' => $project_version, ':hostname' => $ip_address));
    }
  }
}

/**
 * Copy of core's check_plain() function.
 */
function _check_plain($text) {
  return htmlspecialchars($text, ENT_QUOTES);
}

/**
 * Generate an error and exit.
 */
function error($text) {
  echo '<?xml version="1.0" encoding="utf-8"?>'. "\n";
  echo '<error>'. $text ."</error>\n";
  exit(1);
}
