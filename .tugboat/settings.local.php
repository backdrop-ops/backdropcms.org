<?php
// Database.
$database = 'mysql://tugboat:tugboat@mariadb/tugboat';
$database_charset = 'utf8mb4';

// Config.
$config_directories['active'] = '../config/live-active';
$config_directories['staging'] = '../config/staging';

// Trusted hosts.
$settings['trusted_host_patterns'] = array('^.+\.tugboat\.qa$');

// Miscellaneous.
