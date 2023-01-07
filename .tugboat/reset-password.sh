#!/usr/bin/env php
<?php
/**
 * Reset User 1's password.
 *
 * NOTE: This script MUST be run from Backdrop's root directory.
 */

// Bootstrap Backdrop.
define('BACKDROP_ROOT', getcwd());
require_once BACKDROP_ROOT . '/core/includes/bootstrap.inc';
backdrop_bootstrap(BACKDROP_BOOTSTRAP_FULL);

// Reset password.
$password = file_get_contents(getenv('TUGBOAT_ROOT') . '/.tugboat/password');
$user = user_load(1);
$user->pass = $password;
user_save($user);

// Flush caches.
backdrop_flush_all_caches();
