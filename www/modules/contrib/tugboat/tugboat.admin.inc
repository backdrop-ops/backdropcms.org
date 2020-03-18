<?php
/**
 * @file
 * Administrative pages and callbacks for the Tugboat module.
 */

/**
 * The Tugboat administration form.
 */
function tugboat_admin_settings($form, &$form_state) {
  $config = config('tugboat.settings');

  $form['tugboat.qa'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tugboat.qa details'),
  );
  $form['tugboat.qa']['access_token'] = array(
    '#type' => 'textfield',
    '#title' => t('Access token'),
    '#description' => t("An <a href=\"@url\">access token</a> generated from Tugboat.qa. It can alternatively be set in the 'settings' file via <code>\$settings['tugboat_access_token']</code>.", array('@url' => 'https://dashboard.tugboat.qa/access-tokens')),
    '#default_value' => $config->get('access_token'),
    '#size' => 40,
  );
  // Make field disabled if token already set in 'settings' file.
  if (!empty($token = settings_get('tugboat_access_token'))) {
    $form['tugboat.qa']['access_token']['#value'] = substr($token, 0, 4) . str_repeat('*', strlen($token) - 8) . substr($token, -4);
    $form['tugboat.qa']['access_token']['#description'] = t("An <a href=\"@url\">access token</a> generated from Tugboat.qa. This has already been set in the 'settings' file.", array('@url' => 'https://dashboard.tugboat.qa/access-tokens'));
    $form['tugboat.qa']['access_token']['#disabled'] = TRUE;
  }
  $form['tugboat.qa']['build_method'] = array(
    '#type' => 'radios',
    '#title' => t('Build method'),
    '#description' => t("How to build new previews. 'Create' takes longer but allows <a href=\"@build\">build commands</a> to run (exactly which build commands are run depends whether you have a Base Preview set or not), whereas 'Clone' is quicker but skips all build commands.", array('@build' => 'https://docs.tugboat.qa/building-a-preview/preview-deep-dive/how-previews-work/#why-build-phases-matter')),
    '#default_value' => $config->get('build_method'),
    '#options' => array(
      'create' => t('Create'),
      'clone' => t('Clone'),
    ),
  );
  $form['tugboat.qa']['repository_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Repository ID'),
    '#description' => t("The ID of the Tugboat repository to create new previews in. This 24-character, alphanumeric hash can be found in the URL when viewing the repository page in the Tugboat.qa dashboard."),
    '#default_value' => $config->get('repository_id'),
    '#size' => 30,
    '#states' => array(
      'visible' => array(
        ':input[name="build_method"]' => array('value' => 'create'),
      ),
    ),
  );
  $form['tugboat.qa']['git_ref'] = array(
    '#type' => 'textfield',
    '#title' => t('Git Ref'),
    '#description' => t("The git ref to build new previews for. This can be a pull request number, branch, tag, or commit hash."),
    '#default_value' => $config->get('git_ref'),
    '#size' => 30,
    '#states' => array(
      'visible' => array(
        ':input[name="build_method"]' => array('value' => 'create'),
      ),
    ),
  );
  $form['tugboat.qa']['preview_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Preview ID'),
    '#description' => t("The ID of the Tugboat preview to use for cloning new previews from. This 24-character, alphanumeric hash can be found in the URL when viewing the preview page in the Tugboat.qa dashboard."),
    '#default_value' => $config->get('preview_id'),
    '#size' => 30,
    '#states' => array(
      'visible' => array(
        ':input[name="build_method"]' => array('value' => 'clone'),
      ),
    ),
  );
  $form['this_site'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings for this site'),
  );
  $form['this_site']['page_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Page URL'),
    '#description' => t("The URL of the 'Create' and 'Ready' pages for this site."),
    '#default_value' => $config->get('page_url'),
    '#field_prefix' => $_SERVER['SERVER_NAME'] . '/',
  );
  $form['this_site']['create_page_title'] = array(
    '#type' => 'textfield',
    '#title' => t("'Create' page title"),
    '#description' => t("The title of the 'Create' page for this site (where people go to create Tugboat preview sites)."),
    '#default_value' => $config->get('create_page_title'),
  );
  $form['this_site']['create_page_button'] = array(
    '#type' => 'textfield',
    '#title' => t("'Create' page button text"),
    '#description' => t("The button text on the 'Create' page."),
    '#default_value' => $config->get('create_page_button'),
  );
  $form['this_site']['ready_page_title'] = array(
    '#type' => 'textfield',
    '#title' => t("'Ready' page title"),
    '#description' => t("The title of the 'Ready' page for this site (where people are taken when their Tugboat preview site is ready)."),
    '#default_value' => $config->get('ready_page_title'),
  );
  $form['this_site']['preview_site_age'] = array(
    '#type' => 'select',
    '#title' => t('Preview site age'),
    '#description' => t("How long a preview site will be available for. Preview sites older than this will be automatically deleted during cron runs."),
    '#default_value' => $config->get('preview_site_age'),
    '#options' => backdrop_map_assoc(array(
      3600,    // 1 hour
      10800,   // 3 hours
      21600,   // 6 hours
      43200,   // 12 hours
      86400,   // 1 day
      172800,  // 2 days
      259200,  // 3 days
      432000,  // 5 days
      604800,  // 1 week
      1209600, // 2 weeks
      2592000, // 1 month
    ), 'format_interval'),
  );

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save configuration'),
    ),
  );
  return $form;
}

/**
 * Validate handler for tugboat_admin_settings().
 */
function tugboat_admin_settings_validate($form, &$form_state) {
  $values = $form_state['values'];

  // Validate the access token.
  $token = $values['access_token'];
  if (settings_get('tugboat_access_token')) {
    $token = settings_get('tugboat_access_token');
    $form_state['values']['access_token'] = '';
  }
  // We try getting a list of API keys just to test if the access token works.
  $keys = tugboat_api_curl('/keys', array('token' => $token));
  if ($keys['code'] == 401) {
    form_set_error('access_token', t('Invalid access token.'));
  }

  // Validate the repository ID.
  if ($values['build_method'] == 'create') {
    $repo = tugboat_api_curl('/repos/' . $values['repository_id'], array('token' => $token));
    if ($repo['code'] == 404) {
      form_set_error('repository_id', t('Invalid repository ID.'));
    }
  }
  // Validate the preview ID.
  elseif ($values['build_method'] == 'clone') {
    $preview = tugboat_api_curl('/previews/' . $values['preview_id'], array('token' => $token));
    if ($preview['code'] == 404) {
      form_set_error('preview_id', t('Invalid preview ID.'));
    }
  }
}

/**
 * Submit handler for tugboat_admin_settings().
 */
function tugboat_admin_settings_submit($form, &$form_state) {
  form_state_values_clean($form_state);

  // Save values to config.
  $configs = _system_sort_form_values_by_config($form, $form_state, 'tugboat.settings');
  foreach ($configs as $bucket => $data) {
    config_set_multiple($bucket, $data);
  }

  // Rebuild the menu so that the 'Create' and 'Ready' pages are available
  // straight away.
  menu_rebuild();

  backdrop_set_message(t('The configuration options have been saved.'));
  backdrop_set_message(t('Create preview sites here: <a href="/@url">/@url</a>.', array('@url' => $form_state['values']['page_url'])));
}
