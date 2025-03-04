<?php
/**
 * @file
 * Administrative form code for Override Node Options.
 */

/**
 * Settings form.
 */
function override_node_options_settings_form($form, &$form_values) {
  $show_perms = config_get('override_node_options.settings', 'permissions');

  $form = array();
  $form['permissions'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Provide the following permissions:'),
    '#options' => array(
      'general' => t('General: one permission for all content types'),
      'specific' => t('Specific: individual permissions - one for each content type'),
    ),
    '#default_value' => $show_perms,
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['#submit'][] = 'override_node_options_settings_form_submit';

  return $form;
}

/**
 * Submit handler for settings form.
 */
function override_node_options_settings_form_submit(&$form, &$form_state) {
  // Get old perms to compare.
  $old_perms = config_get('override_node_options.settings', 'permissions');
  $new_perms = $form_state['values']['permissions'];

  // Save the setting.
  config_set('override_node_options.settings', 'permissions', $new_perms);

  // Clean up saved permissions.
  $role_names = user_roles();
  $revoke = array();
  $alert = FALSE;

  // If they had specific before and don't now.
  if (!in_array('specific', $new_perms, TRUE) && in_array('specific', $old_perms, TRUE)) {
    $alert = TRUE;
    $permissions = array();
    foreach (node_permissions_get_configured_types() as $type) {
      $permissions += override_node_options_list_permissions($type);
    }

    foreach ($permissions as $permission => $description) {
      $revoke[$permission] = FALSE;
    }

    // Be sure to clear the cache.
    cache_clear_all();
  }

  // If they had general before and don't now.
  if (!in_array('general', $new_perms, TRUE) && in_array('general', $old_perms, TRUE)) {
    $alert = TRUE;
    $revoke = array(
      'override all published option' => FALSE,
      'override all promote to front page option' => FALSE,
      'override all sticky option' => FALSE,
      'override all revision option' => FALSE,
      'enter all revision log entry' => FALSE,
      'override all authored on option' => FALSE,
      'override all authored by option' => FALSE,
    );
    if (module_exists('comment')) {
      $revoke['override all comment setting option'] = FALSE;
    }
  }

  // Any specific grants not used anymore need to be deleted.
  if (!empty($revoke)) {
    foreach ($role_names as $rid => $name) {
      user_role_change_permissions($rid, $revoke);
    }
  }

  // Set a helpful message.
  $message = 'Configuration saved.';
  $arguments = array();
  $status = 'status';

  if ($alert) {
    if (user_access('administer permissions')) {
      $arguments = array('!permissions' => l(t('the permissions page'), 'admin/config/people/permissions', array('fragment' => 'module-override_node_options')));
      $message .= t(' Please visit !permissions and double check access.');
      $status = 'warning';
    }
    else {
      $message .= t(' Please visit the permissions page and double check access.');
    }
  }

  backdrop_set_message(t($message, $arguments), $status);
}
