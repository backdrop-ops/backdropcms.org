<?php

/**
 * @file
 * Administration pages for Luxon module.
 */

/**
 * Administration form.
 */
function luxon_admin_form($form, &$form_state) {
  $config = config('luxon.settings');
  $form['#config'] = 'luxon.settings';

  $form['full_library'] = array(
    '#type' => 'checkbox',
    '#title' => t("Use full library"),
    '#default_value' => $config->get('full_library'),
    '#description' => t("Check this box to use the full library, rather than the minified library. Useful for debugging."),
  );

  return system_settings_form($form);
}
