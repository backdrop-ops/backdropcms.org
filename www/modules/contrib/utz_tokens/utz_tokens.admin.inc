<?php
/**
 * @file
 * Administration pages for User Time Zone Tokens module.
 */

/**
 * Returns the configuration form.
 */
function utz_tokens_admin_form($form, &$form_state) {
  $config = config('utz_tokens.settings');
  $form['#config'] = 'utz_tokens.settings';

  $luxon_enabled = module_exists('luxon');
  if ($luxon_enabled) {
    $msg = t('Luxon module is enabled. You can choose the detection method.');
  }
  else {
    $msg = t('Luxon module is not enabled. Using the default detection method.');
  }
  $form[] = array(
    '#markup' => $msg,
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );

  $options = array(
    UTZ_TOKENS_USER_SERVER => t("Use user's timezone for authenticated users, server timezone for anonymous users."),
    UTZ_TOKENS_USER_BROWSER => t("Use user's timezone for authenticated users, browser's timezone for anonymous users (requires Luxon and Javascript)."),
    UTZ_TOKENS_BROWSER_BROWSER => t("Use the browser's timezone for both authenticated and anonymous users (requires Luxon and Javascript)."),
  );
  $form['timezone_detection'] = array(
    '#type' => 'radios',
    '#title' => t("Timezone Detection"),
    '#options' => $options,
    '#default_value' => $config->get('timezone_detection'),
    '#disabled' => !$luxon_enabled,
  );

  return system_settings_form($form);
}
