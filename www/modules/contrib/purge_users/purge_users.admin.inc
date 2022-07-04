<?php
/**
 * @file
 * Auto Purge Users module config form file.
 */

/**
 * Menu callback function.
 */
function purge_users_config_form($form, &$form_state) {
  $config = config('purge_users.settings');
  $form = array();
  $form['#config'] = 'purge_users.settings';

  // Add stylesheet.
  backdrop_add_css(backdrop_get_path('module', 'purge_users') . "/css/purge_users.css");

  // Confirmation form.
  if (isset($form_state['storage']['confirm'])) {
    $uids = purge_users_get_users_list($form_state);
    // Prevent user 1 from being cancelled.
    if (($key = array_search(1, $uids)) !== false) {
      unset($uids[$key]);
    }
    if (!empty($uids)) {
      $cancel_method = $form_state['values']['purge_users_cancel_method'];
      return purge_users_multiple_cancel_confirm($form, $form_state, $uids, $cancel_method);
    }
    else {
      backdrop_set_message(t('No user account found in the system to purge.'));
    }
  }

  // Fetch user roles and their names.
  $user_roles = user_roles();
  $authenticated_name = $user_roles['authenticated'];
  $anonymous_name = $user_roles['anonymous'];
  unset($user_roles['anonymous']);

  $period_options = array(
    'days' => t('Days'),
    'month' => t('Months'),
    'year' => t('Year'),
  );

  // Never logged in

  $form['never_loggedin_user'] = array(
    '#type' => 'fieldset',
    '#title' => t('Never logged in'),
  );
  $form['never_loggedin_user']['purge_users_enabled_never_loggedin_users'] = array(
    '#type' => 'checkbox',
    '#title' => t('Purge user accounts that have never logged in but have existed for'),
    '#default_value' => $config->get('purge_users_enabled_never_loggedin_users'),
  );
  $form['never_loggedin_user']['purge_users_user_never_lastlogin_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Interval'),
    '#default_value' => $config->get('purge_users_user_never_lastlogin_value'),
    '#prefix' => '<div class="purge-interval-selector clearfix">',
    '#attributes' => array('class' => array('purge-value')),
    '#states' => array(
      'visible' => array(
        ':input[name="purge_users_enabled_never_loggedin_users"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['never_loggedin_user']['purge_users_user_never_lastlogin_period'] = array(
    '#title' => t('Period'),
    '#type' => 'select',
    '#options' => $period_options,
    '#default_value' => $config->get('purge_users_user_never_lastlogin_period'),
    '#attributes' => array('class' => array('purge-period')),
    '#states' => array(
      'visible' => array(
        ':input[name="purge_users_enabled_never_loggedin_users"]' => array('checked' => TRUE),
      ),
    ),
  );

  // Not recently logged in

  $form['not_loggedin_user'] = array(
    '#type' => 'fieldset',
    '#title' => t('Not recently logged in'),
  );
  $form['not_loggedin_user']['purge_users_enabled_loggedin_users'] = array(
    '#type' => 'checkbox',
    '#title' => t('Purge user accounts that logged in at least once but have not logged in for'),
    '#default_value' => $config->get('purge_users_enabled_loggedin_users'),
  );
  $form['not_loggedin_user']['purge_users_user_lastlogin_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Interval'),
    '#default_value' => $config->get('purge_users_user_lastlogin_value'),
    '#prefix' => '<div class="purge-interval-selector clearfix">',
    '#attributes' => array('class' => array('purge-value')),
    '#states' => array(
      'visible' => array(
        ':input[name="purge_users_enabled_loggedin_users"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['not_loggedin_user']['purge_users_user_lastlogin_period'] = array(
    '#title' => t('Period'),
    '#type' => 'select',
    '#options' => $period_options,
    '#default_value' => $config->get('purge_users_user_lastlogin_period'),
    '#attributes' => array('class' => array('purge-period')),
    '#states' => array(
      'visible' => array(
        ':input[name="purge_users_enabled_loggedin_users"]' => array('checked' => TRUE),
      ),
    ),
  );

  // Inactive

  $form['not_active_user'] = array(
    '#type' => 'fieldset',
    '#title' => t('Not activated'),
  );
  $form['not_active_user']['purge_users_enabled_inactive_users'] = array(
    '#type' => 'checkbox',
    '#title' => t('Purge user accounts whose account has not been activated (i.e., currently blocked, never logged in) for'),
    '#default_value' => $config->get('purge_users_enabled_inactive_users'),
  );
  $form['not_active_user']['purge_users_user_inactive_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Interval'),
    '#default_value' => $config->get('purge_users_user_inactive_value'),
    '#prefix' => '<div class="purge-interval-selector clearfix">',
    '#attributes' => array('class' => array('purge-value')),
    '#states' => array(
      'visible' => array(
        ':input[name="purge_users_enabled_inactive_users"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['not_active_user']['purge_users_user_inactive_period'] = array(
    '#title' => t('Period'),
    '#type' => 'select',
    '#options' => $period_options,
    '#default_value' => $config->get('purge_users_user_inactive_period'),
    '#attributes' => array('class' => array('purge-period')),
    '#states' => array(
      'visible' => array(
        ':input[name="purge_users_enabled_inactive_users"]' => array('checked' => TRUE),
      ),
    ),
  );

  // Blocked

  $form['blocked_user'] = array(
    '#type' => 'fieldset',
    '#title' => t('Blocked'),
  );
  $form['blocked_user']['purge_users_enabled_blocked_users'] = array(
    '#type' => 'checkbox',
    '#title' => t('Purge user accounts that have logged in but have been blocked for'),
    '#default_value' => $config->get('purge_users_enabled_blocked_users'),
  );
  $form['blocked_user']['purge_users_user_blocked_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Interval'),
    '#default_value' => $config->get('purge_users_user_blocked_value'),
    '#prefix' => '<div class="purge-interval-selector clearfix">',
    '#attributes' => array('class' => array('purge-value')),
    '#states' => array(
      'visible' => array(
        ':input[name="purge_users_enabled_blocked_users"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['blocked_user']['purge_users_user_blocked_period'] = array(
    '#title' => t('Period'),
    '#type' => 'select',
    '#options' => $period_options,
    '#default_value' => $config->get('purge_users_user_blocked_period'),
    '#attributes' => array('class' => array('purge-period')),
    '#states' => array(
      'visible' => array(
        ':input[name="purge_users_enabled_blocked_users"]' => array('checked' => TRUE),
      ),
    ),
  );

  // Roles

  $form['purge_users_roles'] = array(
    '#title' => t('Limit purge to the following roles'),
    '#description' => t('Select %authenticated_name to allow all roles to be purgeable.', array('%authenticated_name' => $authenticated_name)),
    '#type' => 'checkboxes',
    '#required' => TRUE,
    '#options' => $user_roles,
    '#default_value' => $config->get('purge_users_roles'),
  );
  $form['purge_users_cancel_method'] = array(
    '#type' => 'radios',
    '#title' => t('When cancelling these accounts'),
    '#options' => array(
      'user_cancel_reassign' => t('Delete the account and make its content belong to the %anonymous_name user.', array('%anonymous_name' => $anonymous_name)),
      'user_cancel_delete' => t('Delete the account and its content.'),
    ),
    '#default_value' => $config->get('purge_users_cancel_method'),
  );

  $form['purge_users_purge_on_cron'] = array(
    '#type' => 'checkbox',
    '#title' => t('Purge on cron'),
    '#description' => t('Purging will happen automatically during cron runs according to the criteria above.'),
    '#default_value' => $config->get('purge_users_purge_on_cron'),
  );

  // User notification

  $form['user_notification'] = array(
    '#type' => 'fieldset',
    '#title' => t('User deletion notification'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['user_notification']['purge_users_send_email_notification'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send email notification to purged user accounts.'),
    '#default_value' => $config->get('purge_users_send_email_notification'),
  );
  $form['user_notification']['container'] = array(
    '#type' => 'container',
    '#states' => array(
      'visible' => array(
        ':input[name="purge_users_send_email_notification"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['user_notification']['container']['purge_users_inactive_user_notify_text'] = array(
    '#type' => 'textarea',
    '#title' => t('Body of user notification e-mail'),
    '#default_value' => $config->get('purge_users_inactive_user_notify_text'),
    '#cols' => 70,
    '#rows' => 10,
    '#description' => t('Customize the body of the notification e-mail sent to the user.'),
    '#required' => TRUE,
    '#states' => array(
      'visible' => array(
        ':input[name="purge_users_send_email_notification"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['user_notification']['container']['token_help'] = array(
    '#theme' => 'token_tree',
    '#token_types' => array('user'),
    '#show_restricted' => TRUE,
    '#dialog' => TRUE,
  );

  $form['purge_users_now'] = array(
    '#type' => 'submit',
    '#value' => t('Purge user accounts now'),
    '#name' => 'purge_users_now',
    '#attributes' => array('class' => array('purge-now', 'button-danger')),
    '#submit' => array('purge_users_now_form_submit'),
  );
  $form['#submit'][] = 'purge_users_now_form_submit';
  return system_settings_form($form);
}

/**
 * Validation function for purge user config form.
 */
function purge_users_config_form_validate($form, &$form_state) {
  if (!isset($form_state['storage']['confirm'])) {
    $never_login_value = $form_state['values']['purge_users_user_never_lastlogin_value'];
    $never_login_period = $form_state['values']['purge_users_user_never_lastlogin_period'];
    $login_value = $form_state['values']['purge_users_user_lastlogin_value'];
    $login_period = $form_state['values']['purge_users_user_lastlogin_period'];
    $inactive_value = $form_state['values']['purge_users_user_inactive_value'];
    $inactive_period = $form_state['values']['purge_users_user_inactive_period'];
    $block_value = $form_state['values']['purge_users_user_blocked_value'];
    $block_period = $form_state['values']['purge_users_user_blocked_period'];
    $enable_blocked = $form_state['values']['purge_users_enabled_blocked_users'];
    $never_enable_loggedin = $form_state['values']['purge_users_enabled_never_loggedin_users'];
    $enable_loggedin = $form_state['values']['purge_users_enabled_loggedin_users'];
    $enable_inactive = $form_state['values']['purge_users_enabled_inactive_users'];

    // Validate text field to only contain numeric values.
    if ($never_login_value != '' && !is_numeric($never_login_value)) {
      form_set_error('purge_users_user_never_lastlogin_value', t('Interval must be a number.'));
    }
    if ($login_value != '' && !is_numeric($login_value)) {
      form_set_error('purge_users_user_lastlogin_value', t('Interval must be a number.'));
    }
    if ($inactive_value != '' && !is_numeric($inactive_value)) {
      form_set_error('purge_users_user_inactive_value', t('Interval must be a number.'));
    }
    if ($block_value != '' && !is_numeric($block_value)) {
      form_set_error('purge_users_user_blocked_value', t('Interval must be a number.'));
    }
    // Validate to set purge period more than 1 days.
    if ($never_login_period == 'days' && !empty($never_login_value) && $never_login_value < 1) {
      form_set_error('purge_users_user_never_lastlogin_value', t('Purge period should not be less than 1 day.'));
    }
    if ($login_period == 'days' && !empty($login_value) && $login_value < 1) {
      form_set_error('purge_users_user_lastlogin_value', t('Purge period should not be less than 1 day.'));
    }
    if ($inactive_period == 'days' && !empty($inactive_value) && $inactive_value < 1) {
      form_set_error('purge_users_user_inactive_value', t('Purge period should not be less than 1 day.'));
    }
    if ($block_period == 'days' && !empty($block_value) && $block_value < 1) {
      form_set_error('purge_users_user_blocked_value', t('Purge period should not be less than 1 day.'));
    }
    // Make sure one of the fieldset is checked.
    if ($never_enable_loggedin == 0 && $enable_loggedin == 0 && $enable_inactive == 0 && $enable_blocked == 0) {
      form_set_error('Enable fieldset', t('Please enable one of the Conditions:  Never logged in, Not recently logged in, Not activated, or Blocked.'));
    }
    // Check if value field is not empty.
    if ($never_enable_loggedin == 1 && empty($never_login_value)) {
      form_set_error('purge_users_user_never_lastlogin_value', t('Interval should not be empty.'));
    }
    if ($enable_loggedin == 1 && empty($login_value)) {
      form_set_error('purge_users_user_lastlogin_value', t('Interval should not be empty.'));
    }
    if ($enable_inactive == 1 && empty($inactive_value)) {
      form_set_error('purge_users_user_inactive_value', t('Interval should not be empty.'));
    }
    if ($enable_blocked == 1 && empty($block_value)) {
      form_set_error('purge_users_user_blocked_value', t('Interval should not be empty.'));
    }
  }
}

/**
 * Submit handler for mass-account cancellation form.
 *
 * @see purge_users_config_form()
 */
function purge_users_now_form_submit($form, &$form_state) {
  // Call purge user function to perform operation.
  if ($form_state['triggering_element']['#name'] == 'purge_users_now') {
    config_set('purge_users.settings', 'purge_users_send_email_notification', $form_state['values']['purge_users_send_email_notification']);
    $form_state['storage']['confirm'] = TRUE;
    $form_state['storage']['original_form'] = $form_state['values'];
    $form_state['rebuild'] = TRUE;
  }

  else {
    // Add items to cron queue.
    if ($form_state['values']['purge_users_purge_on_cron'] == 1) {
      backdrop_set_message(t('Purge user operation is scheduled for next cron.'));
    }
  }
}
