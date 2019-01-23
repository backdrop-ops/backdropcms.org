<?php
/**
 * @file
 * Administrative pages and callbacks for the BackdropCMS.org Tugboat module.
 */

/**
 * Form Callback; Provides the settings form for Tugboat settings.
 */
function borg_tugboat_admin_settings($form, &$form_state) {
  $form['#config'] = 'borg_tugboat.settings';
  $config = config('borg_tugboat.settings');

  $tugboat_token = settings_get('borg_tugboat_token');
  $masked_token = '';
  if ($tugboat_token) {
    $masked_token =  substr($tugboat_token, 0, 4) . str_repeat("*", strlen($tugboat_token) - 8) . substr($tugboat_token, -4);
  }

  $form['token'] = array(
    '#type' => 'item',
    '#title' => t('Tugboat Secret Token'),
    '#markup' => ($tugboat_token ? $masked_token : t('Not found! Must be set in settings.php!')),
    '#description' => t('Provides API access to tugboat.qa. This setting must be stored in settings.php as <code>$settings[\'borg_tugboat_token\']</code>'),
  );

  $form['executable_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Tugboat Executable Path'),
    '#default_value' => $config->get('executable_path'),
    '#description' => t('The Tugboat executable binary file location on the server or relative to the Backdrop installation. This file is downloadable from !url.', array('!url' => l('https://dashboard2.tugboat.qa/downloads', 'https://dashboard2.tugboat.qa/downloads'))),
  );

  $form['repository_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Tugboat Repository ID'),
    '#default_value' => $config->get('repository_id'),
    '#description' => t('The repository ID as provided by the Tugboat dashboard. A 24-character alphanumeric hash, such as <code>5bdb5c268eabd5000137a87b</code>.'),
  );

  $form['repository_base'] = array(
    '#type' => 'textfield',
    '#title' => t('Tugboat Repository Base Name'),
    '#default_value' => $config->get('repository_base'),
    '#description' => t('The branch, tag, or pull request name from which to base the previews. A preview with a matching name must be specified in the Tugboat dashboard. Usually <code>master</code> is the latest version.'),
  );

  $form['sandbox_lifetime'] = array(
    '#type' => 'select',
    '#title' => t('Sandbox lifetime'),
    '#default_value' => $config->get('repository_id'),
    '#description' => t('The amount of time the Tugboat preview will be available. Previews older than this will automatically be torn down on cron jobs.'),
    '#options' => backdrop_map_assoc(array(
      7200,    // 2 hours.
      14400,   // 4 hours.
      28800,   // 8 hours.
      86400,   // 1 day.
      172800,  // 2 days.
      259200,  // 3 days.
      345600,  // 4 days.
      432000,  // 5 days.
      518400,  // 6 days.
      604800,  // 1 week.
      1209600, // 2 weeks.
    ), 'format_interval'),
  );

  $form['#validate'] = array('borg_tugboat_admin_settings_validate');
  return system_settings_form($form);
}

/**
 * Validate handler for borg_tugboat_admin_settings().
 */
function borg_tugboat_admin_settings_validate($form, &$form_state) {
  $executable_path = $form_state['values']['executable_path'];

  if (!is_file($executable_path)) {
    form_set_error('executable_path', t('No file found at the provided path.'));
    return;
  }
  elseif (!is_executable($executable_path)) {
    form_set_error('executable_path', t('The Tugboat CLI binary was found, but it is not executable.'));
    return;
  }

  $repo = $form_state['values']['repository_id'];
  //$branch = $form_state['values']['repository_base'];

  $data = array();
  $error_string = '';
  $success = _borg_tugboat_execute("find '$repo'", $data, $error_string, $executable_path);
  if (!$success) {
    if ($error_string) {
      form_set_error('repository_id', t('The provided repository ID was not found. Tugboat returned the response, "!response".', array('!response' => '<code>' . check_plain($error_string) . '</code>')));
    }
    else {
      form_set_error('repository_id', t('Tugboat returned a response that was not understood.'));
    }
  }

}
