<?php
/**
 * @file
 * Local Backdrop CMS configuration file.
 */

/**
 * Simple database settings:
 *
 * Most sites can configure their database by entering the connection string
 * below. If using master/slave or multiple connections, see the advanced
 * database settings.
 */
require_once __DIR__ . '/civicrm_table_prefixes.php';
$databases = array(
  'default' => array (
    'default' =>
    array (
      'driver' => 'mysql',
      'database' => 'backdropcmsorg',
      'username' => '',
      'password' => '',
      'host' => '127.0.0.1',
      'prefix' => $civicrm_table_prefixes,
      'charset' => 'utf8mb4',
      'collation' => 'utf8mb4_general_ci',
    ),
  ),
);

/**
 * Site configuration files location.
 */
$config_directories['active'] = '../config/dev-active';
$config_directories['staging'] = '../config/staging';

/**
 * Trusted host configuration (needed to avoid a status warning).
 */
$settings['trusted_host_patterns'] = array(
  '', // this should be your local site, like '^backdropcms\.org$'
);

/**
 * PRIVATE Environment specific settings.
 */

// Settings for Project GitHub integration. Note, these are deprecated. The
// should be removed after upgrading to the next version of Project module (beta
// 7), which will use the GitHub API module credentials below.
$settings['project_github_secret_key'] = '';
$settings['project_github_access_token'] = '';

// GitHub API module credentials.
$settings['githubapi_client_id'] = '';
$settings['githubapi_client_secret'] = '';
$settings['githubapi_token'] = '';

// BackdropCMS.org GitHub issues configuration.
$settings['borg_github_issues_code_owner_name'] = 'backdrop';
$settings['borg_github_issues_code_repo_name'] = 'backdrop';
$settings['borg_github_issues_repo_fullname'] = 'backdrop/backdrop-issues';

// RobinPanel credentials. Used to set up QA servers on vpn-private.net,
// which is aliased at qa.backdropcms.org.
$settings['rp_api_server'] = 'qa.backdropcms.org';
$settings['rp_api_username'] = '';
$settings['rp_api_password'] = '';

// Gitlc credentials. Used to push QA server information (that was set up by
// RobinPanel) to Gitlc. Gitlc then integrates the information into GitHub.
$settings['gitlc_webhook_api_token'] = '';

// Tugboat.QA credentials. Used to create demo sandboxes.
$settings['tugboat_access_token'] = '';

// MailChimp credentials.
$settings['mailchimp_key'] = '';
$settings['mailchimp_borg_list'] = '';

// SMTP Credentials.
$config['smtp.settings']['smtp_password'] = '';

// Credentials needed for the Newsletter signup block from borg_signup module.
// The define() versions will eventually be deprecated in favor of the
// $settings[] versions.
define('BORG_SIGNUP_KEY', '');
define('BORG_SIGNUP_URL', '');
$settings['borg_signup_key'] = '';
$settings['borg_signup_url'] = '';



