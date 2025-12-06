<?php

/**
 * Implements hook_layout_access_info().
 */
function civicrm_layout_access_info() {
  $info['civicrm'] = array(
    'title' => t('CiviCRM pages'),
    'description' => t('Control access by the current CiviCRM page.'),
    'class' => 'CivicrmLayoutAccess',
  );
  // Add a CiviCRM checksum visibility condition.
  $info['civicrm_checksum'] = array(
    'title' => t('CiviCRM checksum'),
    'description' => t('Has the page been loaded with a valid CiviCRM checksum?'),
    'class' => 'CivicrmChecksumLayoutAccess',
  );
  // Add a URL condition that considers the query parameters in the URL.
  $info['civicrm_urls'] = array(
    'title' => t('CiviCRM URL path and query'),
    'description' => t('Control access by the path and query parameters of the current CiviCRM page.'),
    'class' => 'CivicrmUrlLayoutAccess',
  );
  return $info;
}

/**
 * Implements hook_layout_revert().
 */
function civicrm_layout_revert(Layout $old_layout) {
  if ($old_layout->name == 'civicrm_admin_default') {
    $config = config('layout.layout.' . $old_layout->name);
    $config->delete();
    config_install_default_config('civicrm', 'layout.layout.' . $old_layout->name);
  }
}
