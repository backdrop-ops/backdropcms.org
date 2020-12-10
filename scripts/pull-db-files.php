<?php
/**
 * @file
 * Get the latest sanitized database and/or files for local development.
 *
 * Options:
 *   --database (-d): Download the latest database backup.
 *   --files (-f): Download the latest files directory backup.
 *
 * This requires an `.env` file with the Sanitize website credentials. See the
 * `.example.env` file for details. Make sure to run `lando rebuild -y` after
 * setting up this file.
 */

$user = getenv('SANITIZE_USER');
$pass = getenv('SANITIZE_PASS');
$db_url = 'https://sanitize.backdropcms.org/backdropcms.org/sanitized/backdropcmsorg-latest-sanitized.sql.gz';
$files_url = 'https://sanitize.backdropcms.org/backdropcms.org/files_backups/backdropcmsorg-files-latest.tar.gz';

if (empty($user) || empty($pass)) {
  print "\n\t\033[33mWarning\033[0m: You don't seem to have the credentials set in your \033[1m.env\033[0m file.\n"
    . "\tSet them up and then run: \033[1mlando rebuild -y\033[0m\n"
    . "\tOr request access via Zulip: https://backdrop.zulipchat.com/login/\n\n";

  return 0;
}

if (in_array('--database', $argv) || in_array('-d', $argv)) {
  passthru("wget --http-user=$user --http-password=$pass -c $db_url");
}

if (in_array('--files', $argv) || in_array('-f', $argv)) {
  passthru("wget --http-user=$user --http-password=$pass -c $files_url");
}
