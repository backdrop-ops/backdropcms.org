<?php
/**
 * @file
 * Metatag integration for the metatag_views module.
 */

/**
 * Implements hook_metatag_config_instance_info().
 */
function metatag_views_metatag_config_instance_info() {
  $info['view'] = array('label' => t('Views'));
  return $info;
}
