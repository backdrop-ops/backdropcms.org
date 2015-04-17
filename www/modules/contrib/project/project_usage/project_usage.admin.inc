<?php

/**
 * @file
 * Code necessary for the project_usage administrative UI.
 */

/**
 * Form builder for the module settings page.
 */
function project_usage_settings_form($form, &$form_state) {
  $config = config('project_usage.settings');
  $times = array(
    3 * PROJECT_USAGE_YEAR,
    2 * PROJECT_USAGE_YEAR,
    1 * PROJECT_USAGE_YEAR,
    26 * PROJECT_USAGE_WEEK,
    12 * PROJECT_USAGE_WEEK,
    8 * PROJECT_USAGE_WEEK,
    4 * PROJECT_USAGE_WEEK,
  );
  $age_options = backdrop_map_assoc($times, 'format_interval');
  $form['life_weekly_project'] = array(
    '#type' => 'select',
    '#title' => t('Weekly project data lifespan'),
    '#default_value' => $config->get('life_weekly_project'),
    '#options' => $age_options,
    '#description' => t('Discard the weekly usage for projects after this amount of time has passed.'),
  );
  $form['life_weekly_release'] = array(
    '#type' => 'select',
    '#title' => t('Weekly release data lifespan'),
    '#default_value' => $config->get('life_weekly_release'),
    '#options' => $age_options,
    '#description' => t('Discard the weekly usage for project <em>releases</em> after this amount of time has passed.'),
  );
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submit handler for project_usage_settings_form().
 */
function project_usage_settings_form_submit($form, &$form_state) {
  $config = config('project_usage.settings');
  $config->set('life_weekly_project', (int) $form_state['values']['life_weekly_project']);
  $config->set('life_weekly_project', (int) $form_state['values']['life_weekly_release']);
  backdrop_set_message(t('Configuration saved.'));
}
