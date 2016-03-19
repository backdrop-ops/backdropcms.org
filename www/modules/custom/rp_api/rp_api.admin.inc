<?php
/**
 * @file
 * Administration pages provided by github API module.
 */

 /**
 * Menu callback for admin/config/system/githubapi/settings.
 */
function rp_api_settings() {
  $config = config('rp_api.settings');
  
  // Use setting.php values if exists.
  $server = settings_get('rp_api_server');
  $username = settings_get('rp_api_username');
  $password = settings_get('rp_api_password');

  $form['server'] = array(
    '#type' => 'textfield',
    '#title' => t('Server'),
    '#default_value' => !empty($server) ? $server : $config->get('server'),
    '#description' => t('RobinPanel server IP or hostname.'),
    '#disabled' => !empty($server) ? TRUE : FALSE,
  );

  $form['username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#default_value' => !empty($username) ? $username : $config->get('username'),
    '#description' => t('RobinPanel username.'),
    '#disabled' => !empty($username) ? TRUE : FALSE,
  );

  $form['password'] = array(
    '#type' => 'textfield',
    '#title' => t('Password'),
    '#default_value' => !empty($password) ? $password : $config->get('password'),
    '#description' => t('RobinPanel password.'),
    '#disabled' => !empty($password) ? TRUE : FALSE,
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submit handler for the rp_api_settings() form.
 */
function rp_api_settings_submit($form, &$form_state) {

  $config = config('rp_api.settings');
  
  $server = settings_get('rp_api_server');
  $username = settings_get('rp_api_username');
  $password = settings_get('rp_api_password');

  if(empty($server)){
    $config->set('server', $form_state['values']['server']);
  }
  if(empty($username)){
    $config->set('username', $form_state['values']['username']);
  }
  if(empty($password)){
    $config->set('password', $form_state['values']['password']);
  }
  $config->save();
  
  rp_api_init_robin_panel();
  $cache = rp_api_execute_command('ShowUserInfo', array('user' => $form_state['values']['username']));
  if(FALSE == $cache){
    backdrop_set_message(t('Credentials is not correct !error', array('!error' => rp_api_error_message())), 'error');
  }else{
    backdrop_set_message('<pre>' . print_r($cache, true) . '</pre>');
    backdrop_set_message(t('The configuration options have been saved.'));
  }
}
