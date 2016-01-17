<?php
/**
 * @file
 * Views integration for select_other module.
 */

/**
 * Implementation of hook_views_data_alter().
 */
function select_other_views_data_alter(&$data) {
  $instances = field_read_instances();
  foreach ($instances as $name => $instance) {
    if ($instance['widget']['type'] == 'select_other') {
      $table = 'field_data_' . $instance['field_name'];
      $field = $instance['field_name'] . '_value';
      $data[$table]['entity_id']['field']['handler'] = 'select_other_handler_field';
      $data[$table][$field]['filter']['handler'] = 'select_other_handler_filter';
      $data[$table][$field]['filter']['entity_type'] = $instance['entity_type'];
      $data[$table][$field]['filter']['bundle'] = $instance['bundle'];
    }
  }
}
