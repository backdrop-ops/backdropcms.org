<?php
/**
 * @file
 * Contains administrative configuration page.
 */

/**
 * Administration form for general settings.
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
    // @todo add node forms and comment forms here, use #states below.
  );

  $node_types = node_type_get_types();
  $node_type = array();
  $comment_type = array();
  foreach ($node_types as $type) {
    $node_type[$type->type] = $type->name;
    // @toto check if comments are enabled before including.
    $comment_type[$type->type] = $type->name . ' comments';
  }

  $actions = array(
    'prevent_form' => t('Prevent the form from being submitted until abusive words are removed.'),
    'deactive_form' => t('Submit the form as usual, but then unpublish the content, or block the user.'),
  );
  $form['restrict_abusive_words_actions'] = array(
    '#type' => 'radios',
    '#title' => t('How should abusive words be handled?'),
    '#options' => $actions,
    '#default_value' => $config->get('restrict_abusive_words_actions'),
    '#required' => TRUE,
  );

  $form['restrict_abusive_words_message_validate'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display a message when the form is submitted'),
    '#default_value' => $config->get('restrict_abusive_words_message_validate'),
    '#states' => array(
      'visible' => array(
        ':input[name="restrict_abusive_words_actions"]' => array('value' => 'prevent_form'),
      ),
    ),
  );
  $form['restrict_abusive_words_message_validate_text'] = array(
    '#type' => 'textarea',
    '#title' => t('Validation message text'),
    '#rows' => 2,
    '#default_value' => $config->get('restrict_abusive_words_message_validate_text'),
    '#description' => 'Default value: ' . t('The phrase %word is not allowed. Please replace the phrase and try again.'),
    '#states' => array(
      'visible' => array(
        ':input[name="restrict_abusive_words_actions"]' => array('value' => 'prevent_form'),
        ':input[name="restrict_abusive_words_message_validate"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['restrict_abusive_words_message_submit'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display a message when the form is submitted'),
    '#default_value' => $config->get('restrict_abusive_words_message_submit'),
    '#states' => array(
      'visible' => array(
        ':input[name="restrict_abusive_words_actions"]' => array('value' => 'deactive_form'),
      ),
    ),
  );
  $form['restrict_abusive_words_message_submit_text'] = array(
    '#type' => 'textarea',
    '#title' => t('Submit message text'),
    '#rows' => 2,
    '#default_value' => $config->get('restrict_abusive_words_message_submit_text'),
    '#description' => 'Default value: ' . t('%title has been deactivated because the phrase %word is not allowed.'),
    '#states' => array(
      'visible' => array(
        ':input[name="restrict_abusive_words_actions"]' => array('value' => 'deactive_form'),
        ':input[name="restrict_abusive_words_message_submit"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['restrict_abusive_words_search'] = array(
    '#type' => 'radios',
    '#title' => t('Where to search for abusive words:'),
    '#options' => array(
      'words' => t('Search for complete words only.'),
      'parts' => t('Also Search within words.'),
    ),
    '#default_value' => $config->get('restrict_abusive_words_search'),
    '#required' => TRUE,
    'words' => array(
      '#description' => t('Example: "casino" would not match "casinoroyale".'),
    ),
    'parts' => array(
      '#description' => t('Example: "casino" would match "casinoroyale".'),
    ),
  );

  // @todo - convert to permission.
  $form['restrict_abusive_words_disable_user_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Remove restrictions for the following Roles:'),
    '#options' => $user_role,
    '#default_value' => $config->get('restrict_abusive_words_disable_user_roles'),
  );

  // @todo - convert to permission (or remove entirely, the inverse is above)
  $form['restrict_abusive_words_user_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Restrict the use of abusive words for the selected user roles:'),
    '#description' => t('If no user roles are checked, it will assume all user roles.'),
    '#options' => $user_role,
    '#default_value' => $config->get('restrict_abusive_words_user_roles'),
  );

  $form['restrict_abusive_words_general_form'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Restrict abusive words use on the following general forms:'),
    '#options' => $general_form,
    '#default_value' => $config->get('restrict_abusive_words_general_form'),
  );

  $form['restrict_abusive_words_entity_node'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Restrict abusive words use on the following types of content:'),
    '#options' => $node_type,
    '#default_value' => $config->get('restrict_abusive_words_entity_node'),
  );

  $form['restrict_abusive_words_entity_comment'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Restrict abusive words use on the following types of comments:'),
    '#options' => $comment_type,
    '#default_value' => $config->get('restrict_abusive_words_entity_comment'),
  );

  return system_settings_form($form);
}

/**
 * Add form to add abusive words.
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
 * Validate handler: Add abusive words.
 */
function restrict_abusive_words_admin_add_form_validate($form, &$form_state) {
  $words_list = explode("\n", $form_state['values']['words_list']);
  $words = array_map('trim', $words_list);
  $words = array_filter($words, 'strlen');
  foreach ($words as $word) {
    $search_string = _restrict_abusive_words_get_words_list();
    $check_word = _restrict_abusive_words_exists_words($search_string, $word);
    if ($check_word) {
      $message = t('The word %word already exists in the abusive word list.', array('%word' => $word));
      form_set_error('words_list', $message);
    }
  }
}

/**
 * Submit handler: Add abusive words.
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
 * Confirmation handler: Delete word from abusive word list.
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
 * Submit handler: Delete word from abusive word list.
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
 * Page Callback: List of existing abusive words or phrases.
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
    ->limit(50)
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
 * Page callback: edit Restrict Abusive Word form.
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
 * Submit handler: edit Restrict Abusive Word form.
 */
function restrict_abusive_words_admin_edit_form_submit($form, &$form_state) {
  if (isset($form_state['values'])) {
    backdrop_write_record('restrict_abusive_words', $form_state['values'], 'id');
    $message = 'Updated word: %word';
    $replacements = array('%word' => $form_state['values']['words']);
    watchdog('restrict_abusive_words', $message, $replacements);
    backdrop_set_message(t($message, $replacements));
    $form_state['redirect'] = 'admin/config/content/restrict_abusive_words/list';
    cache_clear_all('*', 'cache_field', TRUE);
    cache_clear_all('restrict_abusive_words', 'cache');
  }
}
