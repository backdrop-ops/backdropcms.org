<?php

/**
 * Implements hook_layout_access_info().
 */
function civicrmtheme_layout_access_info() {
  $info['civicrm'] = array(
    'title' => t('CiviCRM pages'),
    'description' => t('Control access by the current CiviCRM page.'),
    'class' => 'CivicrmLayoutAccess',
  );
  return $info;
}

/**
 * Implements hook_layout_revert().
 */
function civicrmtheme_layout_revert(Layout $old_layout) {
  if ($old_layout->name == 'civicrm_admin_default') {
    $config = config('layout.layout.' . $old_layout->name);
    $config->delete();
    config_install_default_config('civicrmtheme', 'layout.layout.' . $old_layout->name);
  }
}
