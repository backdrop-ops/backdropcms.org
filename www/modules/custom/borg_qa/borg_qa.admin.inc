<?php
/**
 * @file
 * Administration pages provided by github API module.
 */

 /**
 * Menu callback for admin/config/system/qa.
 */
function borg_qa_settings() {
  $config = config('borg_qa.settings');
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submit handler for the qa_settings() form.
 */
function borg_qa_settings_submit($form, &$form_state) {

  $config = config('borg_qa.settings');
  $config->save();
  
  backdrop_set_message(t('The configuration options have been saved.'));
}
