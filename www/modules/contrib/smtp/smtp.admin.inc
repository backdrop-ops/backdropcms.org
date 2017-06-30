<?php
/**
 * @file
 * Administrative page code for the smtp module.
 *
 */

/**
 * Administrative settings.
 *
 * @return
 *   An array containing form items to place on the module settings page.
 */
function smtp_admin_settings() {
  if (config_get('smtp.settings', 'smtp_on')) {
    $smtp_path = backdrop_get_filename('module', 'smtp');
    if ($smtp_path) {
      backdrop_set_message(t('SMTP module is active.'));
    }
    // If backdrop can't find the path to the module, display an error.
    else {
      backdrop_set_message(t("SMTP module error: Can't find file."), 'error');
    }
  }
  else {
    backdrop_set_message(t('SMTP module is INACTIVE.'));
  }

  $form['onoff'] = array(
    '#type'  => 'fieldset',
    '#title' => t('Install options'),
  );
  $form['onoff']['smtp_on'] = array(
    '#type'          => 'radios',
    '#title'         => t('Turn this module on or off'),
    '#default_value' => config_get('smtp.settings', 'smtp_on'),
    '#options'       => array(1 => t('On'), 0 => t('Off')),
    '#description'   => t('To uninstall this module you must turn it off here first.'),
  );
  $form['onoff']['smtp_queue'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Send mail by queue'),
    '#default_value' => config_get('smtp.settings', 'smtp_queue'),
    '#description'   => t('Mails will be sent by Backdrop queue api.'),
  );

  $form['server'] = array(
    '#type'  => 'fieldset',
    '#title' => t('SMTP server settings'),
  );
  $form['server']['smtp_host'] = array(
    '#type'          => 'textfield',
    '#title'         => t('SMTP server'),
    '#default_value' => config_get('smtp.settings', 'smtp_host'),
    '#description'   => t('The address of your outgoing SMTP server.'),
  );
  $form['server']['smtp_hostbackup'] = array(
    '#type'          => 'textfield',
    '#title'         => t('SMTP backup server'),
    '#default_value' => config_get('smtp.settings', 'smtp_hostbackup'),
    '#description'   => t('The address of your outgoing SMTP backup server. If the primary server can\'t be found this one will be tried. This is optional.'),
  );
  $form['server']['smtp_port'] = array(
    '#type'          => 'textfield',
    '#title'         => t('SMTP port'),
    '#size'          => 6,
    '#maxlength'     => 6,
    '#default_value' => config_get('smtp.settings', 'smtp_port'),
    '#description'   => t('The default SMTP port is 25, if that is being blocked try 80. Gmail uses 465. See !url for more information on configuring for use with Gmail.', array('!url' => l(t('this page'), 'http://gmail.google.com/support/bin/answer.py?answer=13287'))),
  );
  // Only display the option if openssl is installed.
  if (function_exists('openssl_open')) {
    $encryption_options = array(
      'standard' => t('No'),
      'ssl'      => t('Use SSL'),
      'tls'      => t('Use TLS'),
    );
    $encryption_description = t('This allows connection to an SMTP server that requires SSL encryption such as Gmail.');
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
    '#default_value' => config_get('smtp.settings', 'smtp_protocol'),
    '#options'       => $encryption_options,
    '#description'   => $encryption_description,
  );

  $form['auth'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('SMTP Authentication'),
    '#description' => t('Leave blank if your SMTP server does not require authentication.'),
  );
  $form['auth']['smtp_username'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Username'),
    '#default_value' => config_get('smtp.settings', 'smtp_username'),
    '#description'   => t('SMTP Username.'),
  );
  $form['auth']['smtp_password'] = array(
    '#type'          => 'password',
    '#title'         => t('Password'),
    '#default_value' => config_get('smtp.settings', 'smtp_password'),
    '#description'   => t('SMTP password. If you have already entered your password before, you should leave this field blank, unless you want to change the stored password.'),
  );

  $form['email_options'] = array(
    '#type'  => 'fieldset',
    '#title' => t('E-mail options'),
  );
  $form['email_options']['smtp_from'] = array(
    '#type'          => 'textfield',
    '#title'         => t('E-mail from address'),
    '#default_value' => config_get('smtp.settings', 'smtp_from'),
    '#description'   => t('The e-mail address that all e-mails will be from.'),
  );
  $form['email_options']['smtp_fromname'] = array(
    '#type'          => 'textfield',
    '#title'         => t('E-mail from name'),
    '#default_value' => config_get('smtp.settings', 'smtp_fromname'),
    '#description'   => t('The name that all e-mails will be from. If left blank will use the site name of:') . ' ' . config_get('smtp.settings', 'site_name'),
  );
  $form['email_options']['smtp_allowhtml'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Allow to send e-mails formated as Html'),
    '#default_value' => config_get('smtp.settings', 'smtp_allowhtml'),
    '#description'   => t('Checking this box will allow Html formated e-mails to be sent with the SMTP protocol.'),
  );

  // If an address was given, send a test e-mail message.
  $test_address = config_get('smtp.settings', 'smtp_test_address');
  if ($test_address != '') {
    // Clear the variable so only one message is sent.
    config_set('smtp.settings', 'smtp_test_address', '');
    global $language;
    $params['subject'] = t('Backdrop SMTP test e-mail');
    $params['body']    = array(t('If you receive this message it means your site is capable of using SMTP to send e-mail.'));
    backdrop_mail('smtp', 'smtp-test', $test_address, $language, $params);

    // Set a friendly message.
    $messsage = t('A test e-mail has been sent to @email.', array('@email' => $test_address));
    if (module_exists('dblog') && user_access('access site reports')) {
      $dbog_url = url('admin/reports/dblog');
      $messsage .= ' ' . t('You may want to <a href="!url">check the logs</a> for any error messages.', array('!url' => $dbog_url));
    }
    backdrop_set_message($messsage);
  }
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

  $form['smtp_debugging'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Enable debugging'),
    '#default_value' => config_get('smtp.settings', 'smtp_debugging'),
    '#description'   => t('Checking this box will print SMTP messages from the server for every e-mail that is sent.'),
  );

  $form['#submit'][] = 'smtp_admin_settings_form_submit';

  // Add a submit button
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Validation for the administrative settings form.
 *
 * @param form
 *   An associative array containing the structure of the form.
 * @param form_state
 *   A keyed array containing the current state of the form.
 */
function smtp_admin_settings_validate($form, &$form_state) {
  if ($form_state['values']['smtp_on'] == 1 && $form_state['values']['smtp_host'] == '') {
    form_set_error('smtp_host', t('You must enter an SMTP server address.'));
  }

  if ($form_state['values']['smtp_on'] == 1 && $form_state['values']['smtp_port'] == '') {
    form_set_error('smtp_port', t('You must enter an SMTP port number.'));
  }

  if ($form_state['values']['smtp_from'] && !valid_email_address($form_state['values']['smtp_from'])) {
    form_set_error('smtp_from', t('The provided from e-mail address is not valid.'));
  }

  // If username is set empty, we must set both username/password empty as well.
  if (empty($form_state['values']['smtp_username'])) {
    $form_state['values']['smtp_password'] = '';
  }

  // A little hack. When form is presentend, the password is not shown (Backdrop way of doing).
  // So, if user submits the form without changing the password, we must prevent it from being reset.
  elseif (empty($form_state['values']['smtp_password'])) {
    unset($form_state['values']['smtp_password']);
  }
}

/**
 * Submit handler().
 */
function smtp_admin_settings_form_submit($form, &$form_state) {
  $config = config('smtp.settings');

  // Check if SMTP status has been changed.
  if ((!config_get('smtp.settings', 'smtp_on') && $form_state['values']['smtp_on'])
     || (config_get('smtp.settings', 'smtp_on') && !$form_state['values']['smtp_on'])) {

    // Get system wide config value for mail system.
    $mail_system = config_get('system.mail', 'default-system');

    // Turning on.
    if ($form_state['values']['smtp_on']) {
      $config->set('smtp_previous_mail_system', $mail_system);
      $mail_system = 'SmtpMailSystem';
    }
    // Turning off.
    else {
      $mail_system = config_get('smtp.settings', 'smtp_previous_mail_system');
    }

    // Update system wide config value for mail_system.
    config_set('system.mail', 'default-system', $mail_system);
  }

  $config->set('smtp_on', $form_state['values']['smtp_on']);
  $config->set('smtp_queue', $form_state['values']['smtp_queue']);
  $config->set('smtp_host', $form_state['values']['smtp_host']);
  $config->set('smtp_hostbackup', $form_state['values']['smtp_hostbackup']);
  $config->set('smtp_port', $form_state['values']['smtp_port']);
  $config->set('smtp_protocol', $form_state['values']['smtp_protocol']);
  $config->set('smtp_username', $form_state['values']['smtp_username']);
  if (isset($form_state['values']['smtp_password'])) {
    $config->set('smtp_password', $form_state['values']['smtp_password']);
  }
  $config->set('smtp_from', $form_state['values']['smtp_from']);
  $config->set('smtp_fromname', $form_state['values']['smtp_fromname']);
  $config->set('smtp_allowhtml', $form_state['values']['smtp_allowhtml']);
  $config->set('smtp_test_address', $form_state['values']['smtp_test_address']);
  $config->set('smtp_debugging', $form_state['values']['smtp_debugging']);
  $config->save();

  backdrop_set_message(t('The configuration options have been saved.'));
}
