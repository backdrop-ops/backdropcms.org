<?php
/**
 * Local settings and sensitive credentials for CiviCRM installation. Change
 * these values for a local installation. This file is not in version control.
 */

/**
 * Backdrop database connection. Put in the local db's username and password.
 */
define( 'CIVICRM_UF_DSN', 'mysql://username:password@localhost/backdropcmsorg?new_link=true');

/**
 * CiviCRM database connection. Put in the local db's username and password.
 */
define('CIVICRM_DSN', 'mysql://username:password@localhost/civicrm?new_link=true');

/**
 * Root of Backdrop CMS. Enter the full path, ending in 'www'.
 */
$cms_root = '';

/**
 * Base URL of the installation. Enter the base URL, including 'http' or 'https'.
 */
define( 'CIVICRM_UF_BASEURL', '');
