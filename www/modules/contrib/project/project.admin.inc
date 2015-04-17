<?php
/**
 * @file
 * Administrative pages for Project module.
 */

/**
 * Form callback; Return the main settings form for Project module.
 */
function project_settings_form($form, &$form_state) {
  $config = config('project.settings');

  $form['allow_machine_name_update'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow changing project shortnames'),
    '#default_value' => $config->get('allow_machine_name_update'),
    '#description' => t('If checked, someone with the right permissions will be able to change the shortname of a project, which also changes the automatically generated URL of that project.'),
  );

  $form['require_lowercase_machine_name'] = array(
    '#type' => 'checkbox',
    '#title' => t('Require lowercase project short names'),
    '#default_value' => $config->get('require_lowercase_machine_name'),
    '#description' => t('If checked, validation will force to use lowercase letter on the project shortname.'),
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submit handler for project_settings_form().
 */
function project_settings_form_submit($form, &$form_state) {
  $config = config('project.settings');
  $config->set('allow_machine_name_update', (bool) $form_state['values']['allow_machine_name_update']);
  $config->set('require_lowercase_machine_name', (bool) $form_state['values']['require_lowercase_machine_name']);
  $config->save();

  backdrop_set_message(t('The configuration options have been saved.'));
}
