<?php

/**
 * @file
 * Administration part (forms and pages) for Spambot module.
 */

/**
 * Form builder for settings form.
 */
function spambot_settings_form() {
  $config = config('spambot.settings');
  $numbers = array(
    1 => 1,
    2 => 2,
    3 => 3,
    4 => 4,
    5 => 5,
    6 => 6,
    7 => 7,
    8 => 8,
    9 => 9,
    10 => 10,
    15 => 15,
    20 => 20,
    30 => 30,
    40 => 40,
    50 => 50,
    60 => 60,
    70 => 70,
    80 => 80,
    90 => 90,
    100 => 100,
    150 => 150,
    200 => 200,
  );

  // Fieldset for set up spam criteria.
  $form['#config'] = 'spambot.settings';
  $form['criteria'] = array(
    '#type' => 'fieldset',
    '#title' => t('Spammer criteria'),
    '#description' => t('A user account or an attempted user registration will be deemed a spammer if the email, username, or IP address has been reported to www.stopforumspam.com more times than the following thresholds.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['criteria']['spambot_criteria_email'] = array(
    '#type' => 'select',
    '#title' => t('Number of times the email has been reported is equal to or more than'),
    '#description' => t('If the email address for a user or user registration has been reported to www.stopforumspam.com this many times, then it is deemed as a spammer.'),
    '#options' => array(0 => t("Don't use email as a criteria")) + $numbers,
    '#default_value' => $config->get('spambot_criteria_email'),
  );
  $form['criteria']['spambot_criteria_username'] = array(
    '#type' => 'select',
    '#title' => t('Number of times the username has been reported is equal to or more than'),
    '#description' => t('If the username for a user or user registration has been reported to www.stopforumspam.com this many times, then it is deemed as a spammer. Be careful about using this option as you may accidentally block genuine users who happen to choose the same username as a known spammer.'),
    '#options' => array(0 => t("Don't use username as a criteria")) + $numbers,
    '#default_value' => $config->get('spambot_criteria_username'),
  );
  $form['criteria']['spambot_criteria_ip'] = array(
    '#type' => 'select',
    '#title' => t('Number of times the IP address has been reported is equal to or more than'),
    '#description' => t('If the IP address for a user or user registration has been reported to www.stopforumspam.com this many times, then it is deemed as a spammer. Be careful about setting this threshold too low as IP addresses can change.'),
    '#options' => array(0 => t("Don't use IP address as a criteria")) + $numbers,
    '#default_value' => $config->get('spambot_criteria_ip'),
  );

  // White lists.
  $form['spambot_whitelist'] = array(
    '#type' => 'fieldset',
    '#title' => t('Whitelists'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['spambot_whitelist']['spambot_whitelist_email'] = array(
    '#type' => 'textarea',
    '#title' => t('Allowed email addresses'),
    '#description' => t('Enter email addresses, one per line.'),
    '#default_value' => $config->get('spambot_whitelist_email'),
  );
  $form['spambot_whitelist']['spambot_whitelist_username'] = array(
    '#type' => 'textarea',
    '#title' => t('Allowed usernames'),
    '#description' => t('Enter usernames, one per line.'),
    '#default_value' => $config->get('spambot_whitelist_username'),
  );
  $form['spambot_whitelist']['spambot_whitelist_ip'] = array(
    '#type' => 'textarea',
    '#title' => t('Allowed IP addresses'),
    '#description' => t('Enter IP addresses, one per line.'),
    '#default_value' => $config->get('spambot_whitelist_ip'),
  );

  // Fieldset for configure protecting at user register form.
  $form['register'] = array(
    '#type' => 'fieldset',
    '#title' => t('User registration'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['register']['spambot_user_register_protect'] = array(
    '#type' => 'checkbox',
    '#title' => t('Protect the user registration form'),
    '#description' => t('If ticked, new user registrations will be tested if they match any known spammers and blacklisted.'),
    '#default_value' => $config->get('spambot_user_register_protect'),
  );

  $sleep_options = array(t("Don't delay"), t('1 second'));
  foreach (array(2, 3, 4, 5, 10, 20, 30) as $num) {
    $sleep_options[$num] = t('@num seconds', array('@num' => $num));
  }
  $form['register']['spambot_blacklisted_delay'] = array(
    '#type' => 'select',
    '#title' => t('If blacklisted, delay for'),
    '#description' => t('If an attempted user registration is blacklisted, you can choose to deliberately delay the request. This can be useful for slowing them down if they continually try to register.<br />Be careful about choosing too large a value for this as it may exceed your PHP max_execution_time.'),
    '#options' => $sleep_options,
    '#default_value' => $config->get('spambot_blacklisted_delay'),
  );

  // Fieldset for set up scanning of existing accounts.
  $form['existing'] = array(
    '#type' => 'fieldset',
    '#title' => t('Scan existing accounts'),
    '#description' => t("This module can also scan existing user accounts to see if they are known spammers. It works by checking user accounts with increasing uid's ie. user id 2, 3, 4 etc during cron."),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['existing']['spambot_cron_user_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum number of user accounts to scan per cron'),
    '#description' => t('Enter the number of user accounts to scan for each cron. If you do not want to scan existing user accounts, leave this as 0.<br />Be careful not to make this value too large, as it will slow your cron execution down and may cause your site to query www.stopforumspam.com more times than allowed each day.'),
    '#size' => 10,
    '#default_value' => $config->get('spambot_cron_user_limit'),
  );
  $form['existing']['spambot_check_blocked_accounts'] = array(
    '#type' => 'checkbox',
    '#title' => t('Scan blocked accounts'),
    '#description' => t('Tick this to scan blocked accounts. Otherwise blocked accounts are not scanned.'),
    '#default_value' => $config->get('spambot_check_blocked_accounts'),
  );
  $form['existing']['spambot_spam_account_action'] = array(
    '#type' => 'select',
    '#title' => t('Action to take'),
    '#description' => t('Please select what action to take for user accounts which are found to be spammers.<br />No action will be taken against accounts with the permission <em>protected from spambot scans</em> but they will be logged.'),
    '#options' => array(
      SPAMBOT_ACTION_NONE => t('None, just log it.'),
      SPAMBOT_ACTION_BLOCK => t('Block user account'),
      SPAMBOT_ACTION_DELETE => t('Delete user account'),
    ),
    '#default_value' => $config->get('spambot_spam_account_action'),
  );

  $form['existing']['spambot_spam_account_nodes_action'] = array(
    '#type' => 'select',
    '#title' => t('Action to take with nodes and comments'),
    '#description' => t('Please select what action to take with nodes and comments for user accounts which are found to be spammers.'),
    '#options' => array(
      SPAMBOT_ACTION_NONE => t('None'),
      SPAMBOT_CONTENT_ACTION_UNPUBLISH => t('Unpublish nodes and comments'),
      SPAMBOT_CONTENT_ACTION_DELETE => t('Delete nodes and comments'),
    ),
    '#default_value' => $config->get('spambot_spam_account_nodes_action'),
  );

  // Get scan status.
  $suffix = '';
  if ($last_uid = $config->get('spambot_last_checked_uid')) {
    $num_checked = db_select('users', 'u')
      ->fields('u', array('uid'))
      ->condition('u.uid', 1, '>')
      ->condition('u.uid', $last_uid, '<=')
      ->countQuery()
      ->execute()
      ->fetchField();

    $num_left = db_select('users', 'u')
      ->fields('u', array('uid'))
      ->condition('u.uid', 1, '>')
      ->condition('u.uid', $last_uid, '>')
      ->countQuery()
      ->execute()
      ->fetchField();

    $last_uid = db_select('users', 'u')
      ->fields('u', array('uid'))
      ->condition('u.uid', 1, '>=')
      ->condition('u.uid', $last_uid, '<=')
      ->orderBy('u.uid', 'DESC')
      ->range(0, 1)
      ->execute()
      ->fetchField();

    $account = user_load($last_uid);
    $suffix = '<br />';
    $suffix .= t('The last checked user account is: !account (uid %uid)', array(
      '!account' => l($account->name, 'user/' . $account->uid),
      '%uid' => $account->uid,
    ));
  }
  else {
    $num_checked = 0;
    $num_left = db_select('users')
      ->fields('users')
      ->condition('uid', 1, '>')
      ->countQuery()
      ->execute()
      ->fetchField();
  }

  $text = t('Accounts checked: %checked, Accounts remaining: %remaining', array('%checked' => $num_checked, '%remaining' => $num_left));
  $form['existing']['message'] = array(
    '#type' => 'fieldset',
    '#title' => t('Scan status'),
    '#description' => $text . $suffix,
  );
  $form['existing']['spambot_last_checked_uid'] = array(
    '#type' => 'textfield',
    '#title' => t('Continue scanning after this user id'),
    '#size' => 10,
    '#description' => t('Scanning of existing user accounts has progressed to, and including, user id @uid and will continue by scanning accounts after user id @uid. If you wish to change where the scan continues scanning from, enter a different user id here. If you wish to scan all users again, enter a value of 0.', array('@uid' => $last_uid)),
    '#default_value' => $last_uid,
  );

  // Fieldset for set up messages which will be displayed for blocked users.
  $form['messages'] = array(
    '#type' => 'fieldset',
    '#title' => t('Blocked messages'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['messages']['spambot_blocked_message_email'] = array(
    '#type' => 'textarea',
    '#title' => t('User registration blocked message (blocked email address)'),
    '#rows' => 1,
    '#default_value' => $config->get('spambot_blocked_message_email'),
    '#description' => t('Message to display when user action is blocked due to email address. <br />Showing a specific reason why registration was blocked may make spambot easier to circumvent.<br />The following tokens are available: <em>@email %email @username %username @ip %ip</em>'),
  );
  $form['messages']['spambot_blocked_message_username'] = array(
    '#type' => 'textarea',
    '#title' => t('User registration blocked message (blocked username)'),
    '#rows' => 1,
    '#default_value' => $config->get('spambot_blocked_message_username'),
    '#description' => t('Message to display when user action is blocked due to username.<br />The following tokens are available: <em>@email %email @username %username @ip %ip</em>'),
  );
  $form['messages']['spambot_blocked_message_ip'] = array(
    '#type' => 'textarea',
    '#title' => t('User registration blocked message (blocked ip address)'),
    '#rows' => 1,
    '#default_value' => $config->get('spambot_blocked_message_ip'),
    '#description' => t('Message to display when user action is blocked due to ip address.<br />The following tokens are available: <em>@email %email @username %username @ip %ip</em>'),
  );

  // Fieldset for configure log rules.
  $form['logging'] = array(
    '#type' => 'fieldset',
    '#title' => t('Log information'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['logging']['spambot_log_blocked_registration'] = array(
    '#type' => 'checkbox',
    '#title' => t('Log information about blocked registrations into Backdrop log'),
    '#default_value' => $config->get('spambot_log_blocked_registration'),
  );

  // StopFormSpam API key.
  $form['spambot_sfs_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('www.stopforumspam.com API key'),
    '#description' => t('If you wish to report spammers to Stop Forum Spam, you need to register for an API key at the <a href="http://www.stopforumspam.com">Stop Forum Spam</a> website.'),
    '#default_value' => $config->get('spambot_sfs_api_key'),
  );

  $form['cache'] = array(
    '#type' => 'fieldset',
    '#title' => t('Caching'),
    '#description' => t('This module can cache the results from  www.stopforumspam.com to reduce the load on the remote server.'),
    '#collapsible' => TRUE,
  );
  $form['cache']['spambot_enable_cache'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable result caching'),
    '#description' => t('If ticked, results from queries to www.stopforumspam.com  will be cached.'),
    '#default_value' => $config->get('spambot_enable_cache'),
  );
  $form['cache']['spambot_cache_expire'] = array(
    '#type' => 'select',
    '#title' => t('Time to expire'),
    '#description' => t('Specify the amount of time to cache the response. See  Backdrop documentation on Caching for when/how entries are removed from the  cache.'),
    '#options' => array(
      CACHE_PERMANENT => t('Keep until explicitly removed'),
      60 => t('1 minute'),
      300 => t('5 minutes'),
      1800 => t('30 minutes'),
      3600 => t('1 hour'),
      86400 => t('1 day'),
      604800 => t('1 week'),
      2592000 => t('1 month (30 days)'),
    ),
    '#default_value' => $config->get('spambot_cache_expire'),
  );
  $form['cache']['spambot_cache_expire_false'] = array(
    '#type' => 'select',
    '#title' => t('Time to expire responses which do not match a spammer'),
    '#description' => t('Specify the amount of time to cache a false response. See  Backdrop documentation on Caching for when/how entries are removed from the  cache.'),
    '#options' => array(
      CACHE_PERMANENT => t('Keep until explicitly removed'),
      60 => t('1 minute'),
      300 => t('5 minutes'),
      1800 => t('30 minutes'),
      3600 => t('1 hour'),
      86400 => t('1 day'),
      604800 => t('1 week'),
      2592000 => t('1 month (30 days)'),
    ),
    '#default_value' => $config->get('spambot_cache_expire_false'),
  );

  return system_settings_form($form);
}
