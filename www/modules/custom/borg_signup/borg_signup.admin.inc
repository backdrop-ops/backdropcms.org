<?php
/**
 * @file
 * Admin page callbacks for the Borg Signup module.
 */

/**
 * Settings form.
 */
function borg_signup_settings_form($form, &$form_state) {
  $config = config('borg_signup.settings');
  $form['#config'] = 'borg_signup.settings';

  $form['threshold'] = array(
    '#type' => 'fieldset',
    '#title' => t("Spam prevention"),
  );
  $form['threshold']['wrapper'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('container-inline')),
  );
  $form['threshold']['wrapper']['borg_signup_threshold_limit'] = array(
    '#type' => 'select',
    '#title' => t('Signup limit'),
    '#title_display' => 'invisible',
    '#options' => backdrop_map_assoc(array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 40, 50, 75, 100, 125, 150, 200, 250, 500)),
    '#default_value' => $config->get('borg_signup_threshold_limit'),
    '#prefix' => '<span aria-hidden="true">' . t('Limit to') . '</span>',
  );
  $form['threshold']['wrapper']['borg_signup_threshold_window'] = array(
    '#type' => 'select',
    '#title' => t('Signup time window'),
    '#title_display' => 'invisible',
    '#options' => array(0 => t('None (disabled)')) + backdrop_map_assoc(array(60, 180, 300, 600, 900, 1800, 2700, 3600, 10800, 21600, 32400, 43200, 86400), 'format_interval'),
    '#default_value' => $config->get('borg_signup_threshold_window'),
    '#prefix' => '<span aria-hidden="true">' . t('messages per') . '</span>',
  );
  $form['threshold']['help'] = array(
    '#type' => 'item',
    '#description' => t("Limit the signups that can be made using borg_signup forms, from the same IP, within a specific window of time. This helps prevent malicious users from performing multiple signups."),
  );

  return system_settings_form($form);
}
