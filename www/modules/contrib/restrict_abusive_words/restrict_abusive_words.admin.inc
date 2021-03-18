<?php

/**
 * @file
 * Contains administrative configuration page.
 */

/**
 * Restrict Abusive Words administration page for general setting.
 */
function restrict_abusive_words_admin_form($form, &$form_state) {
  $config = config('restrict_abusive_words.settings');
  $form = array('#config' => 'restrict_abusive_words.settings');

  $node_type = array();
  $user_role = array();
  $roles = user_roles();
  foreach ($roles as $val_r) {
    $user_role[$val_r] = $val_r;
  }

  $general_form = array(
    'user_register_form' => t('User Registration Form'),
    'user_profile_form' => t('User Profile Form'),
    'webform' => t('Webform'),
  );

  $node_types = node_type_get_types();
  foreach ($node_types as $type) {
    $node_type[$type->type] = $type->name;
  }

  $actions = array(
    'prevent_form' => t('Prevent form submission'),
    'deactive_form' => t('Deactivate the user'),
  );

  $form['restrict_abusive_words_actions'] = array(
    '#type' => 'radios',
    '#title' => t('Choose the action.'),
    '#description' => t('Choose the action after submitting the form, it will prevent the form from being submitted or deactivate the content/user etc.'),
    '#options' => $actions,
    '#default_value' => $config->get('restrict_abusive_words_actions'),
  );

  $form['restrict_abusive_words_disable_user_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Disable User Roles'),
    '#description' => t('Disable Restrict Abusive Words for the selected user roles. By default no user role is selected.'),
    '#options' => $user_role,
    '#default_value' => $config->get('restrict_abusive_words_disable_user_roles'),
  );

  $form['restrict_abusive_words_user_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Enable User Roles'),
    '#description' => t('Restrict the use of abusive words for the selected user roles. If no user roles are checked, it will assume all user roles.'),
    '#options' => $user_role,
    '#default_value' => $config->get('restrict_abusive_words_user_roles'),
  );

  $form['restrict_abusive_words_general_form'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select General Form'),
    '#description' => t('Restrict abusive words use in General Form.'),
    '#options' => $general_form,
    '#default_value' => $config->get('restrict_abusive_words_general_form'),
  );

  $form['restrict_abusive_words_entity_node'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select Node Form'),
    '#description' => t('Restrict abusive words use in Node Form.'),
    '#options' => $node_type,
    '#default_value' => $config->get('restrict_abusive_words_entity_node'),
  );

  $form['restrict_abusive_words_entity_comment'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Select Comment Node Form'),
    '#description' => t('Restrict abusive words use in Comment Node Form.'),
    '#options' => $node_type,
    '#default_value' => $config->get('restrict_abusive_words_entity_comment'),
  );

  return system_settings_form($form);
}

/**
 * Admin Add form to add abusive words.
 */
function restrict_abusive_words_admin_add_form($form, &$form_state) {
  $form = array();

  $form['check_word'] = array(
    '#type' => 'textfield',
    '#title' => t('Look up abusive word'),
    '#description' => t('Look up abusive word.'),
    '#maxlength' => 60,
    '#autocomplete_path' => 'admin/config/content/restrict_abusive_words/autocomplete',
  );

  $form['words_list'] = array(
    '#type' => 'textarea',
    '#title' => t('Words'),
    '#description' => t("Enter a word or phrase you want to restrict as abusive. You can enter multiple words by adding words on a new line."),
    '#required' => TRUE,
  );

  $form['save_wordlist'] = array(
    '#type' => 'submit',
    '#value' => t('Add abusive word'),
  );

  $form['cancel_wordlist'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
  );

  return $form;
}

/**
 * Validate handler to add abusive words.
 */
function restrict_abusive_words_admin_add_form_validate($form, &$form_state) {
  $words_list = explode("\n", $form_state['values']['words_list']);
  $words = array_map('trim', $words_list);
  $words = array_filter($words, 'strlen');
  foreach ($words as $word) {
    $search_string = _restrict_abusive_words_get_words_list();
    $check_word = _restrict_abusive_words_exists_words($search_string, $word);
    if ($check_word) {
      form_set_error('words_list', $word . ' already exists in the abusive word list.');
    }
  }
}

/**
 * Submit handler to add abusive words.
 */
function restrict_abusive_words_admin_add_form_submit($form, &$form_state) {
  $words_list = explode("\n", $form_state['values']['words_list']);
  $words = array_map('trim', $words_list);
  $words = array_filter($words, 'strlen');
  foreach ($words as $word) {
    $row = new stdClass();
    $row->words = $word;
    backdrop_write_record('restrict_abusive_words', $row);
    watchdog('restrict_abusive_words', 'Added word: %word', array('%word' => $row->words));
    backdrop_set_message(t('Added word: %word', array('%word' => $row->words)));
  }
  $form_state['redirect'] = 'admin/config/content/restrict_abusive_words/add';
  cache_clear_all('*', 'cache_field', TRUE);
  cache_clear_all('restrict_abusive_words', 'cache');
}

/**
 * Confirmation callback to delete word from abusive word list.
 */
function restrict_abusive_words_admin_form_delete_confirm($form, &$form_state, $word_id) {
  $form = array();
  $form['word_id'] = array(
    '#type' => 'value',
    '#value' => $word_id,
  );
  return confirm_form($form, t('Are you sure you want to delete this word or phrase from the abusive word list?'), 'admin/config/content/restrict_abusive_words/list');
}

/**
 * Submit callback to delete word from abusive word list.
 */
function restrict_abusive_words_admin_form_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    db_delete('restrict_abusive_words')
      ->condition('id', $form_state['values']['word_id'])
      ->execute();

    backdrop_set_message(t('The word was removed from the abusive word list'));
    $form_state['redirect'] = 'admin/config/content/restrict_abusive_words/list';
    cache_clear_all('*', 'cache_field', TRUE);
    cache_clear_all('restrict_abusive_words', 'cache');
  }
}

/**
 * Lists of existing abusive words or phrases that are configured to restrict.
 */
function restrict_abusive_words_admin_list() {
  $build = array();
  $header = array(
    array('data' => t('Word'), 'field' => 'words', 'sort' => 'asc'),
    array('data' => t('Operations'), 'colspan' => 2),
  );
  $rows = array();

  $list = db_select('restrict_abusive_words', 'w')
    ->fields('w')
    ->extend('PagerDefault')
    ->extend('TableSort')
    ->orderByHeader($header)
    ->limit(10)
    ->execute();

  foreach ($list as $word) {
    $rows[] = array(
      check_plain($word->words),
      l(t('Edit word'), 'admin/config/content/restrict_abusive_words/edit/' . $word->id),
      l(t('Delete word'), 'admin/config/content/restrict_abusive_words/delete/' . $word->id),
    );
  }
  $build['table'] = array(
    '#markup' => theme('table', array('header' => $header, 'rows' => $rows)),
  );
  $build['pager'] = array(
    '#markup' => theme('pager'),
  );

  return $build;
}

/**
 * Restrict Abusive Word edit word form.
 */
function restrict_abusive_words_admin_edit_form($form, &$form_state, $word_id = NULL) {
  if (!isset($word_id) || !is_numeric($word_id)) {
    backdrop_set_message(t('The restrict_abusive_words ID of the word or phrase you are trying to edit is missing or is not a number.'), 'error');
    backdrop_goto('admin/config/content/restrict_abusive_words/list');
  }

  $word = db_select('restrict_abusive_words', 'w')
    ->fields('w')
    ->condition('w.id', $word_id)
    ->execute()
    ->fetch();

  $form = array();
  $form['id'] = array(
    '#type' => 'hidden',
    '#value' => $word->id,
  );

  $form['words'] = array(
    '#type' => 'textfield',
    '#title' => t('Word or phrase to Edit'),
    '#default_value' => $word->words,
    '#description' => t('Enter the word or phrase you want to update.'),
    '#size' => 50,
    '#maxlength' => 255,
    '#required' => TRUE,
  );

  $form['update_word'] = array(
    '#type' => 'submit',
    '#value' => t('Save word'),
  );
  $form['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
  );

  return $form;
}

/**
 * Restrict Abusive Words edit form submit handler.
 */
function restrict_abusive_words_admin_edit_form_submit($form, &$form_state) {
  if (isset($form_state['values'])) {
    backdrop_write_record('restrict_abusive_words', $form_state['values'], 'id');
    watchdog('restrict_abusive_words', 'Updated word: %word', array('%word' => $form_state['values']['words']));
    backdrop_set_message(t('Updated word: %word', array('%word' => $form_state['values']['words'])));
    $form_state['redirect'] = 'admin/config/content/restrict_abusive_words/list';
    cache_clear_all('*', 'cache_field', TRUE);
    cache_clear_all('restrict_abusive_words', 'cache');
  }
}
