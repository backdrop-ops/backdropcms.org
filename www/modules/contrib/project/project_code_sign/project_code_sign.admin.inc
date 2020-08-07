<?php
/**
 * @file
 * Administrative pages for Project Code Sign module.
 */

/**
 * Form callback; Settings for Project Code Sign.
 */
function project_code_sign_admin_form($form, &$form_state) {
  $signing_profiles = code_sign_get_signing_profiles();

  $headers = array(
    'name' => t('Name'),
    'crypto_engine' => t('Engine'),
    'profile_id' => t('Profile ID'),
  );

  $rows = array();
  foreach($signing_profiles as $key => $profile) {
    $rows[$key] = array(
      'name' => check_plain($profile['name']),
      'crypto_engine' => $profile['crypto_engine'],
      'profile_id' => check_plain($key),
    );
  }

  $form['#config'] = 'project_codesign.settings';
  $form['title'] = array(
    '#type' => 'item',
    '#title' => t('Enabled Signing Profiles'),
    '#description' => t('Check the profiles that should be used to sign modules.'),
  );
  $form['enabled_profiles'] = array(
    '#type' => 'tableselect',
    '#header' => $headers,
    '#options' => $rows,
    '#empty' => t('No signing keys found.'),
    '#default_value' => config('project_codesign.settings')->get('enabled_profiles'),
  );

  return system_settings_form($form);
}
