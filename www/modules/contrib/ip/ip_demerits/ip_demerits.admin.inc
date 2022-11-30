<?php
/**
 * @file
 * IP Demerits administrative functions.
 */

/**
 * Return settings form for IP Demerits module.
 */
function ip_demerits_settings_form($form, &$form_state) {
  $form['#config'] = 'ip_demerits.settings';
  $config = config('ip_demerits.settings');

  $form[] = array(
    '#type' => 'help',
    '#markup' => t('Configure demerits for automatic application based on user actions.'),
  );

  $form['points'] = array(
    '#type' => 'fieldset',
    '#title' => t('Points'),
    '#description' => t('Enter the demerit points to award for each type of demerit.'),
  );
  $form['points']['points_manual_default'] = array(
    '#type' => 'number',
    '#title' => t('Manual default'),
    '#description' => t('Default points for a manual demerit.'),
    '#min' => 0,
    '#step' => 1,
    '#default_value' => (int) $config->get('points_manual_default'),
  );
  $form['points']['points_user_blocked'] = array(
    '#type' => 'number',
    '#title' => t('User blocked'),
    '#description' => t('Points for demerit when a user account has been blocked.'),
    '#min' => 0,
    '#step' => 1,
    '#default_value' => (int) $config->get('points_user_blocked'),
  );
  $form['points']['points_user_cancelled'] = array(
    '#type' => 'number',
    '#title' => t('User cancelled'),
    '#description' => t('Points for demerit when a user account has been cancelled.'),
    '#min' => 0,
    '#step' => 1,
    '#default_value' => (int) $config->get('points_user_cancelled'),
  );

/*
  // TODO: add automatic actions to take place when a user or IP address's
  // demerit points exceed a specified threshold.

  $form['take_actions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Actions'),
    '#description' => t('Enter the threshold points for demerit-based actions.'),
  );
  $form['take_actions']['threshold_block_ip'] = array(
    '#type' => 'number',
    '#title' => t('Block IP threshold'),
    '#description' => t('Enter the threshold demerits to block an IP address.'),
    '#min' => 0,
    '#step' => 1,
    '#default_value' => (int) $config->get('threshold_block_ip'),
  );
 */

  return system_settings_form($form);
}

/**
 * Assign a demerit to a user or IP address.
 */
function ip_demerits_create_form($form, &$form_state) {

  $form[] = array(
    '#type' => 'help',
    '#markup' => t('Assign a demerit to a user/IP combination.'),
  );

  $form['account'] = array(
    '#type' => 'textfield',
    '#title' => t('User account'),
    '#description' => t('Enter the user account to assign the demerit to.'),
    '#min' => 0,
    '#step' => 1,
    '#autocomplete_path' => 'ip_demerits_account_autocomplete',
    '#ajax' => array(
      'callback' => 'ip_demerits_account_ips_callback',
      'wrapper' => 'ip-demerits-account-ips',
    ),
  );

  $account_ips = array(0 => t('—Select—'));
  $account_text = isset($form_state['input']['account']) ? $form_state['input']['account'] : '';
  $account_parts = explode('|', $account_text);
  $two_parts = count($account_parts) == 2;
  $uid = 0;
  if (count($account_parts) == 2) {
    $uid = trim($account_parts[1]);
    $user_records = ip_tracker_user_records($uid);
    foreach ($user_records as $record) {
      $account_ips[$record->ip] = $record->ip;
    }
  }
  $default_ip = isset($form_state['input']['ip']) ? $form_state['input']['ip'] : 0;
  $form['ip'] = array(
    '#type' => 'select',
    '#title' => t('IP address'),
    '#description' => t('Select one of these IP addresses used by the chosen user.'),
    '#options' => $account_ips,
    '#default_value' => $default_ip,
    '#prefix' => '<div id="ip-demerits-account-ips">',
    '#suffix' => '</div>',
  );

  $form['points'] = array(
    '#type' => 'number',
    '#title' => t('Points'),
    '#description' => t('Enter the number of points to assign to the demerit.'),
    '#min' => 1,
    '#step' => 1,
    '#default_value' => config_get('ip_demerits.settings', 'points_manual_default'),
  );
  $form['reason'] = array(
    '#type' => 'textfield',
    '#title' => t('Reason'),
    '#description' => t('Enter a short reason for the demerit.'),
    '#size' => 80,
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Assign demerit'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/people/ip-demerits',
    '#attributes' => array(
      'class' => array('button', 'button-secondary', 'form-submit'),
    ),
  );

  return $form;
}

/**
 * Autocomplete callback for user account.
 */
function ip_demerits_account_autocomplete($string) {
  if (empty($string)) {
    return '';
  }
  $matches = array();
  $query = db_select('users', 'u');
  $return = $query
    ->fields('u', array('name', 'uid', 'status'))
    ->condition('u.name', ('%' . db_like($string) . '%'), 'LIKE')
    ->range(0, 10)
    ->execute();
  foreach ($return as $row) {
    $entry = "{$row->name} | {$row->uid}";
    $entry_sanitized = check_plain($entry);
    $matches[$entry] = $entry_sanitized;
  }
  backdrop_json_output($matches);
}

/**
 * Callback for account IPs. Return the form element with the list of IP
 * addresses for the selected user account.
 */
function ip_demerits_account_ips_callback($form, $form_state) {
  return $form['ip'];
}

/**
 * Validation function for assigning a demerit to a user or IP address.
 */
function ip_demerits_create_form_validate($form, &$form_state) {
  // Validate the username and user ID
  $account_text = $form_state['values']['account'];
  $account_parts = explode('|', $account_text);
  if (count($account_parts) != 2) {
    form_set_error('account', t('%account is not a valid entry. Please select a value from the autocomplete field.', array('%account' => $account_text)));
  }
  else {
    $name = trim($account_parts[0]);
    $uid = trim($account_parts[1]);
    $account = user_load($uid);
    if (!$account || $name != $account->name) {
      form_set_error('account', t('%account is not a valid entry. Please select a value from the autocomplete field.', array('%account' => $account_text)));
    }

    // Validate the IP address
    $ip = $form_state['values']['ip'];
    $user_records = ip_tracker_user_records($uid);
    $user_ip = FALSE;
    foreach ($user_records as $record) {
      if($ip == $record->ip) {
        $user_ip = TRUE;
        break;
      }
    }
    if (!$user_ip) {
      form_set_error('ip', t('%ip is not an IP address used by account %name.', array('%name' => $name)));
    }
    $form_state['values']['uid'] = $uid;
  }
}

/**
 * Submission function for assigning a demerit to a user or IP address.
 */
function ip_demerits_create_form_submit($form, &$form_state) {
  $uid = $form_state['values']['uid'];
  $ip = $form_state['values']['ip'];
  $points = $form_state['values']['points'];
  $reason = $form_state['values']['reason'];
  ip_demerits_assign(ip2long($ip), $uid, $points, $reason);
  $account = user_load($uid);
  if ($uid && $account) {
    backdrop_set_message(format_plural($points, 'A demerit of @points point has been assigned to user %name and IP address %ip for the reason %reason.', 'A demerit of @points points has been assigned to user %name and IP address %ip for the reason %reason.', array(
      '@points' => $points,
      '%name' => $name,
      '%ip' => $ip,
      '%reason' => $reason)));
  }
  else {
    backdrop_set_message(format_plural($points, 'A demerit of @points point has been assigned to IP address %ip for the reason %reason.', 'A demerit of @points points has been assigned to IP address %ip for the reason %reason.', array(
      '@points' => $points,
      '%ip' => $ip,
      '%reason' => $reason)));
  }
}
