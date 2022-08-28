<?php

require_once 'airmail.civix.php';
use CRM_Airmail_Utils as E;


/**
 * Implements hook_civicrm_alterMailParams().
 */
function airmail_civicrm_alterMailParams(&$params, $context) {
  $backend = E::getBackend();
  if (!$backend || !in_array('CRM_Airmail_Backend', class_implements($backend))) {
    return;
  }

  $backend->alterMailParams($params, $context);
}

/**
 * hook_civicrm_navigationMenu
 *
 * add "Airmail Configuration" to the Mailings menu
 */
function airmail_civicrm_navigationMenu(&$menu) {

  $adder = new CRM_Airmail_NavAdd($menu);

  $attributes = array(
    'label' => ts('Airmail Configuration'),
    'name' => 'Airmail Configuration',
    'url' => 'civicrm/airmail/settings',
    'permission' => 'access CiviMail,administer CiviCRM',
    'operator' => 'AND',
    'separator' => 1,
    'active' => 1,
  );
  $adder->addItem($attributes, array('Mailings'));
  $menu = $adder->getMenu();
}

/**
 * Implements hook_civicrm_config().
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_config
 */
function airmail_civicrm_config(&$config) {
  _airmail_civix_civicrm_config($config);
}

/**
 * Implements hook_civicrm_xmlMenu().
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_xmlMenu
 */
function airmail_civicrm_xmlMenu(&$files) {
  _airmail_civix_civicrm_xmlMenu($files);
}

/**
 * Implements hook_civicrm_install().
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_install
 */
function airmail_civicrm_install() {
  _airmail_civix_civicrm_install();
}

/**
 * Implements hook_civicrm_postInstall().
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_postInstall
 */
function airmail_civicrm_postInstall() {
  _airmail_civix_civicrm_postInstall();
}

/**
 * Implements hook_civicrm_uninstall().
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_uninstall
 */
function airmail_civicrm_uninstall() {
  _airmail_civix_civicrm_uninstall();
}

/**
 * Implements hook_civicrm_enable().
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_enable
 */
function airmail_civicrm_enable() {
  _airmail_civix_civicrm_enable();
}

/**
 * Implements hook_civicrm_disable().
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_disable
 */
function airmail_civicrm_disable() {
  _airmail_civix_civicrm_disable();
}

/**
 * Implements hook_civicrm_upgrade().
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_upgrade
 */
function airmail_civicrm_upgrade($op, CRM_Queue_Queue $queue = NULL) {
  return _airmail_civix_civicrm_upgrade($op, $queue);
}

/**
 * Implements hook_civicrm_managed().
 *
 * Generate a list of entities to create/deactivate/delete when this module
 * is installed, disabled, uninstalled.
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_managed
 */
function airmail_civicrm_managed(&$entities) {
  _airmail_civix_civicrm_managed($entities);
}

/**
 * Implements hook_civicrm_caseTypes().
 *
 * Generate a list of case-types.
 *
 * Note: This hook only runs in CiviCRM 4.4+.
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_caseTypes
 */
function airmail_civicrm_caseTypes(&$caseTypes) {
  _airmail_civix_civicrm_caseTypes($caseTypes);
}

/**
 * Implements hook_civicrm_angularModules().
 *
 * Generate a list of Angular modules.
 *
 * Note: This hook only runs in CiviCRM 4.5+. It may
 * use features only available in v4.6+.
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_angularModules
 */
function airmail_civicrm_angularModules(&$angularModules) {
  _airmail_civix_civicrm_angularModules($angularModules);
}

/**
 * Implements hook_civicrm_alterSettingsFolders().
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_alterSettingsFolders
 */
function airmail_civicrm_alterSettingsFolders(&$metaDataFolders = NULL) {
  _airmail_civix_civicrm_alterSettingsFolders($metaDataFolders);
}
