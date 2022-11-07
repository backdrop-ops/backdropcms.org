<?php
/**
 * @file
 * Administrative page code for the smtp module.
 */

/**
 * Administrative settings.
 */
function smtp_admin_settings() {
  $config = config('smtp.settings');
  if ($config->get('smtp_on')) {
    backdrop_set_message(t('SMTP.module is active.'));
  }
  else {
    backdrop_set_message(t('SMTP module is INACTIVE.'));
  }

  $logging = $config->get('smtp_debugging');

  $form['#config'] = 'smtp.settings';
  $form['onoff'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Install options'),
  );
  $form['onoff']['smtp_on'] = array(
    '#type'          => 'radios',
    '#title'         => t('Turn this module on or off'),
    '#default_value' => $config->get('smtp_on'),
    '#options'       => array(1 => t('On'), 0 => t('Off')),
    '#description'   => t('To uninstall this module you must turn it off here first.'),
  );
  $form['onoff']['smtp_deliver'] = array(
    '#type'          => 'radios',
    '#title'         => t('Turn on delivery of emails'),
    '#default_value' => $config->get('smtp_deliver'),
    '#options'       => array(1 => t('On'), 0 => t('Off')),
    '#description'   => t('With this option turned off, email messages will be queued up and processed as normal, but not actually delivered. This option should only be used for testing purposes.'),
  );
  $form['onoff']['smtp_queue'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Send mail by queue'),
    '#default_value' => $config->get('smtp_queue'),
    '#description'   => t('Mails will be sent by Backdrop queue api.'),
  );
  $form['onoff']['smtp_queue_fail'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Retry sending mail on error.'),
    '#default_value' => $config->get('smtp_queue_fail'),
    '#description'   => t('Mails will be added to the queue and sent by backdrop queue api.'),
  );

  $form['server'] = array(
    '#type'  => 'fieldset',
    '#title' => t('SMTP server settings'),
  );
  $form['server']['smtp_host'] = array(
    '#type'          => 'textfield',
    '#title'         => t('SMTP server'),
    '#default_value' => $config->get('smtp_host'),
    '#description'   => t('The address of your outgoing SMTP server.'),
  );
  $form['server']['smtp_hostbackup'] = array(
    '#type'          => 'textfield',
    '#title'         => t('SMTP backup server'),
    '#default_value' => $config->get('smtp_hostbackup'),
    '#description'   => t('The address of your outgoing SMTP backup server. If the primary server can\'t be found this one will be tried. This is optional.'),
  );
  $form['server']['smtp_port'] = array(
    '#type'          => 'textfield',
    '#title'         => t('SMTP port'),
    '#size'          => 6,
    '#maxlength'     => 6,
    '#default_value' => $config->get('smtp_port'),
    '#description'   => t('The default SMTP port is 25, if that is being blocked try 80. Gmail uses 465. See !url for more information on configuring for use with Gmail.', array('!url' => l(t('this page'), 'http://gmail.google.com/support/bin/answer.py?answer=13287'))),
  );

  // Only display the option if openssl is installed.
  if (function_exists('openssl_open')) {
    $encryption_options = array(
      'standard' => t('No'),
      'ssl'      => t('Use SSL'),
      'tls'      => t('Use TLS'),
    );
    $encryption_description = t('This allows connection to a SMTP server that requires SSL encryption such as Gmail.');
  }
  // If openssl is not installed, use normal protocol.
  else {
    config_set('smtp.settings', 'smtp_protocol', 'standard');
    $encryption_options = array('standard' => t('No'));
    $encryption_description = t('Your PHP installation does not have SSL enabled. See the !url page on php.net for more information. Gmail requires SSL.', array('!url' => l(t('OpenSSL Functions'), 'http://php.net/openssl')));
  }
  $form['server']['smtp_protocol'] = array(
    '#type'          => 'select',
    '#title'         => t('Use encrypted protocol'),
    '#default_value' => $config->get('smtp_protocol'),
    '#options'       => $encryption_options,
    '#description'   => $encryption_description,
  );

  if (module_exists('smtp_oauth')) {
    $form['oauth'] = array(
      '#type'        => 'fieldset',
      '#title'       => t('SMTP OAuth Authentication'),
      '#description' => t('Set proper values for oauth authentication.'),
    );
    $gauth_account_id = config_get('smtp_oauth.settings', 'smtp_oauth_google_api_client_id');
    $gauth_account = FALSE;
    if ($gauth_account_id) {
      $gauth_account = gauth_account_load($gauth_account_id, FALSE);
      if ($gauth_account['is_authenticated']) {
        backdrop_set_message(t('Google account is authenticated, ready to send'));
      }
      else {
        $authenticate_link = l(t('Authenticate now'), '/gauth/response_handler/' . $gauth_account_id, array('attributes' => array('target' => "_blank")));
        backdrop_set_message(t('You need to authenticate with Google, !link', array('!link' => $authenticate_link)), 'warning');
      }
    }
    $form['oauth']['smtp_oauth_google_api_client_id'] = array(
      '#type'          => 'hidden',
      '#value' => $gauth_account_id,
      '#config' => 'smtp_oauth.settings',
    );
    $form['oauth']['smtp_oauth_client_id'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Client Id'),
      '#default_value' => $gauth_account ? $gauth_account['client_id'] : '',
      '#description'   => t(' OAuth Client ID.'),
    );
    $form['oauth']['smtp_oauth_client_secret'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Client Secret'),
      '#default_value' => $gauth_account ? $gauth_account['client_secret'] : '',
      '#description'   => t(' OAuth Client Secret.'),
    );
    $form['oauth']['smtp_oauth_email_address'] = array(
      '#type'          => 'textfield',
      '#title'         => t('OAuth Email Address'),
      '#default_value' => config_get('smtp_oauth.settings', 'smtp_oauth_email_address'),
      '#description'   => t('Use the gmail address which is authenticated.'),
      '#config' => 'smtp_oauth.settings',
    );
  }
  else {
    $form['auth'] = array(
      '#type'        => 'fieldset',
      '#title'       => t('SMTP Authentication'),
      '#description' => t('Leave blank if your SMTP server does not require authentication.'),
    );
    $form['auth']['smtp_username'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Username'),
      '#default_value' => $config->get('smtp_username'),
      '#description'   => t('SMTP Username.'),
    );
    $form['auth']['smtp_password'] = array(
      '#type'          => 'password',
      '#title'         => t('Password'),
      '#default_value' => $config->get('smtp_password'),
      '#description'   => t('SMTP password. If you have already entered your password before, you should leave this field blank, unless you want to change the stored password.'),
      '#attributes'    => array(
        'autocomplete' => 'off',
      ),
    );
  }

  $form['email_options'] = array(
    '#type'  => 'fieldset',
    '#title' => t('E-mail options'),
  );
  $form['email_options']['smtp_from'] = array(
    '#type'          => 'textfield',
    '#title'         => t('E-mail from address'),
    '#default_value' => $config->get('smtp_from'),
    '#description'   => t('The e-mail address that all e-mails will be from.'),
  );
  $form['email_options']['smtp_fromname'] = array(
    '#type'          => 'textfield',
    '#title'         => t('E-mail from name'),
    '#default_value' => $config->get('smtp_fromname'),
    '#description'   => t('The name that all e-mails will be from. If left blank will use the site name of:') . ' ' . $config->get('site_name'),
  );
  $form['email_options']['smtp_allowhtml'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Allow to send e-mails formatted as Html'),
    '#default_value' => $config->get('smtp_allowhtml'),
    '#description'   => t('Checking this box will allow Html formatted e-mails to be sent with the SMTP protocol.'),
  );

  $form['client'] = array(
    '#type'  => 'fieldset',
    '#title' => t('SMTP client settings'),
  );
  $form['client']['smtp_client_hostname'] = array(
    '#type' => 'textfield',
    '#title' => t('Hostname'),
    '#default_value' => $config->get('smtp_client_hostname'),
    '#description' => t('The hostname to use in the Message-Id and Received headers, and as the default HELO string. Leave blank for using %server_name.', array('%server_name' => isset($_SERVER['SERVER_NAME']) ? $_SERVER['SERVER_NAME'] : 'localhost.localdomain')),
  );
  $form['client']['smtp_client_helo'] = array(
    '#type' => 'textfield',
    '#title' => t('HELO'),
    '#default_value' => $config->get('smtp_client_helo'),
    '#description' => t('The SMTP HELO/EHLO of the message. Defaults to hostname (see above).'),
  );

  $form['email_test'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Send test e-mail'),
  );
  $form['email_test']['smtp_test_address'] = array(
    '#type'          => 'textfield',
    '#title'         => t('E-mail address to send a test e-mail to'),
    '#default_value' => '',
    '#description'   => t('Type in an address to have a test e-mail sent there.'),
  );

  $form['debugging'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Debugging and logging'),
  );

  $logging_options = array(
    SMTP_LOGGING_ALL => t('Log everything'),
    SMTP_LOGGING_ERRORS => t('Errors only'),
    SMTP_LOGGING_NONE => t('No logging'),
  );
  $form['debugging']['smtp_debugging'] = array(
    '#type'          => 'select',
    '#title'         => t('Logging'),
    '#options'       => $logging_options,
    '#default_value' => $logging,
    '#description'   => t('Choose the appropriate log level. "Log everything" will log errors and informational messages when an email is sent. "Errors only" will only create a log entry when sending failed. "No logging" will disable all logging for this module.'),
  );
  $form['email_test']['smtp_reroute_address'] = array(
    '#type'          => 'textfield',
    '#title'         => t('E-mail address to reroute all emails to'),
    '#default_value' => $config->get('smtp_reroute_address'),
    '#description'   => t('All emails sent by the site will be rerouted to this email address; use with caution.'),
  );

  $form['debugging']['maillog'] = array(
    '#type' => 'fieldset',
    '#title' => t('Maillog integration'),
  );

  if (!module_exists('maillog')) {
    $form['debugging']['maillog']['#description'] = t('Installing the <a href="@url">Maillog module</a> also allows keeping copies of all emails sent through the site.', array('@url' => 'https://www.backdropcms.org/project/maillog'));
  }
  else {
    $form['debugging']['maillog']['#description'] = t('The <a href="@url">Maillog module</a> is installed, it can also be used to keep copies of all emails sent through the site.', array('@url' => url('admin/config/development/maillog')));

    $form['debugging']['maillog']['maillog_log'] = array(
      '#type' => 'checkbox',
      '#title' => t("Create table entries in maillog table for each e-mail."),
      '#default_value' => config_get('maillog.settings', 'maillog_log'),
    );

    $form['debugging']['maillog']['maillog_devel'] = array(
      '#type' => 'checkbox',
      '#title' => t("Display the e-mails on page using devel module (if enabled)."),
      '#default_value' => config_get('maillog.settings', 'maillog_devel'),
      '#disabled' => !module_exists('devel'),
    );
  }

  $form['#submit'][] = 'smtp_admin_settings_form_submit';
  $form = system_settings_form($form);
  $form['#submit'][] = 'smtp_admin_settings_submit_post_system_settings';
  return $form;
}

/**
 * Validation for the administrative settings form.
 */
function smtp_admin_settings_validate($form, &$form_state) {
  if ($form_state['values']['smtp_on'] == 1 && $form_state['values']['smtp_host'] == '') {
    form_set_error('smtp_host', t('You must enter a SMTP server address.'));
  }

  if ($form_state['values']['smtp_on'] == 1 && $form_state['values']['smtp_port'] == '') {
    form_set_error('smtp_port', t('You must enter a SMTP port number.'));
  }

  if ($form_state['values']['smtp_from'] && !valid_email_address($form_state['values']['smtp_from'])) {
    form_set_error('smtp_from', t('The provided from e-mail address is not valid.'));
  }
}

/**
 * Submit handler().
 */
function smtp_admin_settings_form_submit($form, &$form_state) {
  $config = config('smtp.settings');

  // Check if SMTP status has been changed.
  if ((!$config->get('smtp_on') && $form_state['values']['smtp_on'])
     || ($config->get('smtp_on') && !$form_state['values']['smtp_on'])) {

    // Get system wide config value for mail system.
    $mail_system = config_get('system.mail', 'default-system');

    // Turning on.
    if ($form_state['values']['smtp_on']) {
      $config->set('smtp_previous_mail_system', $mail_system);

      if (module_exists('smtp_oauth')) {
        $mail_system = 'SmtpOAuthMailSystem';
      }
      else {
        $mail_system = 'SmtpMailSystem';
      }
    }
    // Turning off.
    else {
      $mail_system = $config->get('smtp_previous_mail_system');
    }

    $config->save();
    // Update system wide config value for mail_system.
    config_set('system.mail', 'default-system', $mail_system);
  }

  // If username is set empty, we must set both username/password empty as well.
  if (empty($form_state['values']['smtp_username'])) {
    $form_state['values']['smtp_password'] = '';
  }

  // A little hack. When form is presentend, the password is not shown (Backdrop
  // way of doing). So, if user submits the form without changing the password,
  // we must prevent it from being reset.
  elseif (empty($form_state['values']['smtp_password'])) {
    unset($form_state['values']['smtp_password']);
  }

  if (module_exists('smtp_oauth')) {
    // Save Google Api Client.
    if ($form_state['values']['smtp_oauth_google_api_client_id']) {
      $google_api_client = gauth_account_load($form_state['values']['smtp_oauth_google_api_client_id'], FALSE);
      $google_api_client['client_id'] = $form_state['values']['smtp_oauth_client_id'];
      $google_api_client['client_secret'] = $form_state['values']['smtp_oauth_client_secret'];
      gauth_account_save($google_api_client);
    }
    else {
      $account = [
        'name' => 'SMTP Google Api Client',
        'client_id' => $form_state['values']['smtp_oauth_client_id'],
        'client_secret' => $form_state['values']['smtp_oauth_client_secret'],
        'access_token' => '',
        'services' => ['gmail'],
        'developer_key' => '',
        'access_type' => 'offline',
        'is_authenticated' => FALSE,
      ];
      $google_api_client = gauth_account_save($account);
      if (isset($google_api_client['is_error'])) {
        backdrop_set_message(t('@name:', array('@name' => $account['name'])) . $google_api_client['message'], 'error');
      }
      else {
        config_set('smtp_oauth.settings', 'smtp_oauth_google_api_client_id', $google_api_client['id']);
      }
    }
    $form_state['values']['smtp_oauth_google_api_client_id'] = $google_api_client['id'];
    unset($form_state['values']['smtp_oauth_client_id']);
    unset($form_state['values']['smtp_oauth_client_secret']);
  }

  // Save the test address to send an email after all the settings have been
  // updated.
  $form_state['storage']['smtp']['smtp_test_address'] = $form_state['values']['smtp_test_address'];
  unset($form_state['values']['smtp_test_address']);
}

/**
 * Submit handler for the administrative settings form containing all
 * functionality to be run after system_settings_form_submit.
 */
function smtp_admin_settings_submit_post_system_settings($form, &$form_state) {
  // If an address was given, send a test e-mail message.
  $test_address = $form_state['storage']['smtp']['smtp_test_address'];
  if ($test_address != '') {
    $language = language_default();
    $params['subject'] = t('Backdrop SMTP test e-mail');
    $params['body'] = array(t('If you receive this message it means your site is capable of using SMTP to send e-mail.'));
    backdrop_mail('smtp', 'smtp-test', $test_address, $language, $params);
    backdrop_set_message(t('A test e-mail has been sent to @email. You may want to !check for any error messages.', array('@email' => $test_address, '!check' => l(t('check the logs'), 'admin/reports/dblog'))));
  }
}
