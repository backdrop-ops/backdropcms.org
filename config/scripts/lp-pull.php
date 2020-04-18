<?php
/**
 * @file
 * Get the latest sanitized database for local development.
 *
 * This will only work if you have the creds set up in your .env file.
 *
 * If you don't have those creds, but would like to contribute to the
 * backdropcms.org code base please request them on:
 * Zulip: https://backdrop.zulipchat.com/#narrow/stream/218635-Backdrop
 * 
 * or file a new issue on 
 * https://github.com/backdrop-ops/backdropcms.org/issues/
 *
 * requesting either a copy of the DB or access to creds.
 *
 * Allotting access to creds will be made on a case by case basis.
 *
 * If you have the creds make sure to set them in your .env file and
 * do a `lando rebuild -y` to let lando load the env vars into the app.
 */

$user = getenv('LP_USER');
$pass = getenv('LP_PASS');
$db_url = getenv('LP_DB_URL');
$files_url = getenv('LP_FILES_URL');

if (empty($user) || empty($pass) || empty($db_url)) {
  print "\n\t\033[33mWarning\033[0m: You don't seem to have your credentials"
    . " set in your \033[1m.env\033[0m file.\n\n"
    . "\tIf you have them then set them and: \033[1mlando rebuild -y\033[0m\n\n"
    . "\tIf you don't have them, but want them file an issue at:\n"
    . "\thttps://github.com/backdrop-ops/backdropcms.org/issues/\n\n"
    . "\tRequesting the credentials or a one time copy of a sanitized"
    . " database.\n\n";

  return 0;
}

if (in_array('--database', $argv) || in_array('-d', $argv)) {
  passthru(
    "wget --http-user=$user --http-password=$pass -c $db_url"
  );
}

if (in_array('--files', $argv) || in_array('-f', $argv)) {
  passthru(
    "wget --http-user=$user --http-password=$pass -c $files_url"
  );
}
