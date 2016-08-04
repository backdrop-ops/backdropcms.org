<?php
/**
 * @file
 * Administration pages provided by github API module.
 */

/**
 * Menu callback for admin/config/system/githubapi/settings.
 */
function githubapi_admin_settings() {
  $config = config('githubapi.settings');

  // Use setting.php values if exists.
  $client_id = settings_get('githubapi_client_id');
  $client_secret = settings_get('githubapi_client_secret');

  $form['client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Client ID'),
    '#default_value' => !empty($client_id) ? $client_id : $config->get('client_id'),
    '#description' => t('Github Application Client ID.'),
    '#disabled' => !empty($client_id) ? TRUE : FALSE,
  );

  $form['client_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Client secret'),
    '#default_value' => !empty($client_secret) ? $client_secret : $config->get('client_secret'),
    '#description' => t('Github Application Client Secret.'),
    '#disabled' => !empty($client_secret) ? TRUE : FALSE,
  );
  $form['actions']['#type'] = 'actions';

  // Allow save button only if values not stored in settings.php
  if (empty($client_id) || empty($client_secret)) {
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save configuration'),
    );
  }

  $token = settings_get('githubapi_token');
  if (empty($token)) {
    $token = $config->get('token');
  }

  if (empty($token)) {
    $form['actions']['auth'] = array(
      '#type' => 'submit',
      '#value' => t('Authorize an Application'),
      '#submit' => array('githubapi_link_user_submit'),
    );
  }
  else {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Revoke authorization for an Application'),
      '#submit' => array('githubapi_revoke_submit'),
    );
  }

  return $form;
}

/**
 * Submit handler for the githubapi_admin_settings() form.
 */
function githubapi_admin_settings_submit($form, &$form_state) {

  $config = config('githubapi.settings');

  $client_id = settings_get('githubapi_client_id');
  $client_secret = settings_get('githubapi_client_secret');

  // Store only if value not stored in settings.php.
  if (empty($client_id)) {
    $config->set('client_id', $form_state['values']['client_id']);
  }

  // Store only if value not stored in settings.php.
  if (empty($client_secret)) {
    $config->set('client_secret', $form_state['values']['client_secret']);
  }
  $config->save();
  backdrop_set_message(t('The configuration options have been saved.'));
}

/**
 * Get token handler for the githubapi_admin_settings() form.
 */
function githubapi_link_user_submit($form, &$form_state) {
  global $base_url;

  $config = config('githubapi.settings');
  $params = array();

  $client_id = settings_get('githubapi_client_id');
  if (empty($client_id)) {
    $client_id = $config->get('client_id');
  }
  $params['client_id'] = $client_id;

  $params['scope'] = GITHUB_SCOPE;

  $params['redirect_uri'] = $base_url . '/githubapi/register';

  $query = http_build_query($params);

  $link = GITHUB_URI . '/login/oauth/authorize?' . $query;

  backdrop_goto($link, array('absolute' => TRUE));

}

/**
 * Revoke authorization for an application.
 */
function githubapi_revoke_submit($form, &$form_state) {

  $config = config('githubapi.settings');

  $client_id = settings_get('githubapi_client_id');
  if (empty($client_id)) {
    $client_id = $config->get('client_id');
  }

  $client_secret = settings_get('githubapi_client_secret');
  if (empty($client_secret)) {
    $client_secret = $config->get('client_secret');
  }
  $token = settings_get('githubapi_token');
  if (empty($token)) {
    $token = $config->get('token');
  }

  $url = GITHUB_API_URI . '/applications/' . $client_id . '/tokens/' . $token;

  $options['headers']['Authorization'] = 'Basic ' . base64_encode($client_id . ':' . $client_secret);
  $options['method'] = 'DELETE';
  $result = backdrop_http_request($url, $options);

  switch ($result->code) {
    case 204:
      backdrop_set_message(t('Authorization for an application deleted'));
      $config->set('token', '');
      $config->save();
      break;
    case 401:
      backdrop_set_message(t('Unexpected Basic authorization error. Make sure that client_id and client_secret is properly set.'), 'error');
      break;
    default:
      backdrop_set_message(t('Unexpected behaviour. Debug data: status_message = !error.', array('!error' => $result->error), 'error'));

  }
}


/**
 * Menu callback; Process an Github authentication.
 */
function githubapi_get_access_token() {

  if (isset($_GET['code'])) {
    $config = config('githubapi.settings');

    $client_id = settings_get('githubapi_client_id');
    if (empty($client_id)) {
      $client_id = $config->get('client_id');
    }

    $client_secret = settings_get('githubapi_client_secret');
    if (empty($client_secret)) {
      $client_secret = $config->get('client_secret');
    }

    $params = array();
    $params['client_id'] = $client_id;
    $params['client_secret'] = $client_secret;
    $params['code'] = filter_xss($_GET['code']);

    $query = http_build_query($params);
    $response = file_get_contents(GITHUB_URI . '/login/oauth/access_token?' . $query);

    parse_str($response, $result);

    if (isset($result['access_token'])) {
      $token = $result['access_token'];
      $config->set('token', $token);
      $config->save();
      backdrop_set_message(t('We connected to GitHub now .'));
      backdrop_goto('admin/config/system/githubapi/settings');
    }
  }
  // If we didn't get a token or code, connection to Github failed.
  backdrop_set_message(t('Failed connecting to GitHub.'), 'error');
  backdrop_goto('admin/config/system/githubapi/settings');
}
