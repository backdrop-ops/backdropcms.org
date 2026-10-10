<?php

/**
 * @file
 * Provides various callbacks for the whole core module integration.
 */

/**
 * Callback for getting properties of an entity.
 */
function entity_plus_metadata_entity_plus_get_properties($entity, array $options, $name, $entity_type) {
  if ($name == 'url') {
    $return = entity_uri($entity_type, $entity);
    $return += array(
      'path' => NULL,
      'options' => array()
    );
    return url($return['path'], $return['options'] + $options);
  }
}

/**
 * Callback for getting book node properties.
 * @see entity_plus_metadata_book_entity_plus_info_alter()
 */
function entity_plus_metadata_book_get_properties($node, array $options, $name, $entity_type) {
  switch ($name) {
    case 'book':
      if (isset($node->book['bid'])) {
        return $node->book['bid'];
      }
      return NULL;

    case 'book_ancestors':
      $ancestors = array();
      while (!empty($node->book['plid'])) {
        $link = book_link_load($node->book['plid']);
        array_unshift($ancestors, $link['nid']);
        $node = node_load($link['nid']);
      }
      return $ancestors;
  }
}

/**
 * Callback for getting comment properties.
 * @see entity_plus_metadata_comment_entity_plus_info_alter()
 */
function entity_plus_metadata_comment_get_properties($comment, array $options, $name) {
  switch ($name) {
    case 'name':
      return $comment->name;

    case 'mail':
      if ($comment->uid != 0) {
        $account = user_load($comment->uid);
        return $account->mail;
      }
      return $comment->mail;

    case 'edit_url':
      return url('comment/edit/' . $comment->cid, $options);

    case 'parent':
      if (!empty($comment->pid)) {
        return $comment->pid;
      }
      // There is no parent comment.
      return NULL;
  }
}

/**
 * Callback for setting comment properties.
 * @see entity_plus_metadata_comment_entity_plus_info_alter()
 */
function entity_plus_metadata_comment_setter($comment, $name, $value) {
  switch ($name) {
    case 'node':
      $comment->nid = $value;
      // Also set the bundle name.
      $node = node_load($value);
      $comment->node_type = 'comment_node_' . $node->type;
      break;
  }
}

/**
 * Callback for getting comment related node properties.
 * @see entity_plus_metadata_comment_entity_plus_info_alter()
 */
function entity_plus_metadata_comment_get_node_properties($node, array $options, $name, $entity_type) {
  switch ($name) {
    case 'comment_count':
      return isset($node->comment_count) ? $node->comment_count : 0;

    case 'comment_count_new':
      return comment_num_new($node->nid);

    case 'comments':
      $select = db_select('comment', 'c')
        ->fields('c', array('cid'))
        ->condition('c.nid', $node->nid);
      return array_keys($select->execute()->fetchAllKeyed(0, 0));
  }
}

/**
 * Getter callback for getting global languages.
 */
function entity_plus_metadata_locale_get_languages($data, array $options, $name) {
  return isset($GLOBALS[$name]) ? $GLOBALS[$name]->langcode : NULL;
}

/**
 * Getter callback for getting the preferred user language.
 */
function entity_plus_metadata_locale_get_user_language($account, array $options, $name) {
  return user_preferred_language($account)->langcode;
}

/**
 * Return the options lists for the node and comment status property.
 */
function entity_plus_metadata_status_options_list() {
  return array(
    NODE_PUBLISHED => t('Published'),
    NODE_NOT_PUBLISHED => t('Unpublished'),
  );
}

/**
 * Return the options lists for the node and comment status property.
 */
function entity_plus_metadata_node_comment_settings_options_list() {
  return array(
    COMMENT_NODE_OPEN => t('Open'),
    COMMENT_NODE_CLOSED => t('Closed'),
    COMMENT_NODE_HIDDEN => t('Closed and hidden'),
  );
}

/**
 * Callback for getting node properties.
 *
 * @see entity_plus_metadata_node_entity_plus_info_alter()
 */
function entity_plus_metadata_node_get_properties($node, array $options, $name, $entity_type) {
  switch ($name) {
    case 'is_new':
      return empty($node->nid) || !empty($node->is_new);

    case 'source':
      if (!empty($node->tnid) && $source = node_load($node->tnid)) {
        return $source;
      }
      return NULL;

    case 'edit_url':
      return url('node/' . $node->nid . '/edit', $options);

    case 'author':
      return !empty($node->uid) ? $node->uid : backdrop_anonymous_user();
  }
}

/**
 * Callback for determing access for node revision related properties.
 */
function entity_plus_metadata_node_revision_access($op, $name, $entity = NULL, $account = NULL) {
  return $op == 'view' ? user_access('view revisions', $account) : user_access('administer nodes', $account);
}

/**
 * Callback for getting poll properties.
 * @see entity_plus_metadata_poll_entity_plus_info_alter()
 */
function entity_plus_metadata_poll_node_get_properties($node, array $options, $name) {
  $total_votes = $highest_votes = 0;
  foreach ($node->choice as $choice) {
    if ($choice['chvotes'] > $highest_votes) {
      $winner = $choice;
      $highest_votes = $choice['chvotes'];
    }
    $total_votes = $total_votes + $choice['chvotes'];
  }

  if ($name == 'poll_duration') {
    return $node->runtime;
  }
  elseif ($name == 'poll_votes') {
    return $total_votes;
  }
  elseif (!isset($winner)) {
    // There is no poll winner yet.
    return NULL;
  }
  switch ($name) {
    case 'poll_winner_votes':
      return $winner['chvotes'];

    case 'poll_winner':
      return $winner['chtext'];

    case 'poll_winner_percent':
      return ($winner['chvotes'] / $total_votes) * 100;
  }
}

/**
 * Callback for getting statistics properties.
 * @see entity_plus_metadata_statistics_entity_plus_info_alter()
 */
function entity_plus_metadata_statistics_node_get_properties($node, array $options, $name) {
  $statistics = (array) statistics_get($node->nid);
  $statistics += array('totalcount' => 0, 'daycount' => 0, 'timestamp' => NULL);

  switch ($name) {
    case 'views':
      return $statistics['totalcount'];

    case 'day_views':
      return $statistics['daycount'];

    case 'last_view':
      return $statistics['timestamp'];
  }
}

/**
 * Access callback for restricted node statistics properties.
 */
function entity_plus_metadata_statistics_properties_access($op, $property, $entity = NULL, $account = NULL) {
  if ($property == 'views' && user_access('view post access counter', $account)) {
    return TRUE;
  }
  return user_access('access statistics', $account);
}

/**
 * Callback for getting site-wide properties.
 * @see entity_plus_metadata_system_entity_plus_info_alter()
 */
function entity_plus_metadata_system_get_properties($data, array $options, $name) {
  $system_core_config = config_get('system.core');
  switch ($name) {
    case 'name':
      return !empty($system_core_config['site_name']) ? $system_core_config['site_name'] : 'Backdrop';

    case 'url':
      return url('<front>', $options);

    case 'login_url':
      return url('user', $options);

    case 'current_user':
      return $GLOBALS['user']->uid ? $GLOBALS['user']->uid : backdrop_anonymous_user();

    case 'current_date':
      return REQUEST_TIME;

    case 'current_page':
      // Subsequent getters of the struct retrieve the actual values.
      return array();

    default:
      return !empty($system_core_config['site_' . $name]) ? $system_core_config['site_' . $name] : '';
  }
}

/**
 * Callback for getting properties for the current page request.
 * @see entity_plus_metadata_system_entity_plus_info_alter()
 */
function entity_plus_metadata_system_get_page_properties($data, array $options, $name) {
  switch ($name) {
    case 'url':
      return $GLOBALS['base_root'] . request_uri();
  }
}

/**
 * Callback for getting file properties.
 * @see entity_plus_metadata_system_entity_plus_info_alter()
 */
function entity_plus_metadata_system_get_file_properties($file, array $options, $name) {
  switch ($name) {
    case 'name':
      return $file->filename;

    case 'mime':
      return $file->filemime;

    case 'size':
      return $file->filesize;

    case 'url':
      return url(file_create_url($file->uri), $options);

    case 'owner':
      return $file->uid;
  }
}

/**
 * Callback for getting term properties.
 *
 * @see entity_plus_metadata_taxonomy_entity_plus_info_alter()
 */
function entity_plus_metadata_taxonomy_term_get_properties($term, array $options, $name) {
  switch ($name) {
    case 'node_count':
      return count(taxonomy_select_nodes($term->tid));

    case 'description':
      return check_markup($term->description, isset($term->format) ? $term->format : NULL, '', TRUE);

    case 'parent':
      if (isset($term->parent[0]) && !is_array(isset($term->parent[0]))) {
        return $term->parent;
      }
      return array_keys(taxonomy_term_load_parents($term->tid));

    case 'parents_all':
      // We have to return an array of ids.
      $tids = array();
      foreach (taxonomy_term_load_parents_all($term->tid) as $parent) {
        $tids[] = $parent->tid;
      }
      return $tids;

    case 'vocabulary':
      return taxonomy_vocabulary_load($term->vocabulary);
  }
}

/**
 * Callback for getting raw term properties.
 */
function entity_plus_metadata_taxonomy_term_get_raw_properties($term, array $options, $name) {
  switch ($name) {
    case 'vocabulary':
      return $term->vocabulary;
  }
}

/**
 * Callback for setting term properties.
 *
 * @see entity_plus_metadata_taxonomy_entity_plus_info_alter()
 */
function entity_plus_metadata_taxonomy_term_setter($term, $name, $value) {
  switch ($name) {
    case 'vocabulary':
      return $term->vocabulary = $value;

    case 'parent':
      return $term->parent = $value;
  }
}

/**
 * Callback for getting vocabulary properties.
 * @see entity_plus_metadata_taxonomy_entity_plus_info_alter()
 */
function entity_plus_metadata_taxonomy_vocabulary_get_properties($vocabulary, array $options, $name) {
  switch ($name) {
    case 'term_count':
      $sql = "SELECT COUNT (1) FROM {taxonomy_term_data} td WHERE td.vocabulary = :vocabulary";
      return db_query($sql, array(':vocabulary' => $vocabulary->machine_name))->fetchField();
  }
}

/**
 * Callback for getting user properties.
 * @see entity_plus_metadata_user_entity_plus_info_alter()
 */
function entity_plus_metadata_user_get_properties($account, array $options, $name, $entity_type) {
  switch ($name) {
    case 'last_access':
      // In case there was no access the value is 0, but we have to return NULL.
      return empty($account->access) ? NULL : $account->access;

    case 'last_login':
      return empty($account->login) ? NULL : $account->login;

    case 'name':
      return empty($account->uid) ? (!empty(config_get('system.core', 'anonymous')) ? config_get('system.core', 'anonymous') : t('Anonymous')) : $account->name;

    case 'url':
      if (empty($account->uid)) {
        return NULL;
      }
      $return = entity_uri('user', $account);
      return $return ? url($return['path'], $return['options'] + $options) : '';

    case 'edit_url':
      return empty($account->uid) ? NULL : url("user/$account->uid/edit", $options);

    case 'roles':
      return isset($account->roles) ? $account->roles : array();

    case 'theme':
      return empty($account->theme) ? (!empty(config_get('system.core', 'system_default')) ? config_get('system.core', 'system_default') : 'basis') : $account->theme;
  }
}

/**
 * Callback for setting user properties.
 * @see entity_plus_metadata_user_entity_plus_info_alter()
 */
function entity_plus_metadata_user_set_properties($account, $name, $value) {
  switch ($name) {
    case 'roles':
      $account->roles = array_intersect(array_keys(user_roles()), $value);
      break;
  }
}

/**
 * Options list callback returning all user roles.
 */
function entity_plus_metadata_user_roles($property_name = 'roles', $info = array(), $op = 'edit') {
  $roles = user_roles();
  if ($op == 'edit') {
    unset($roles[BACKDROP_AUTHENTICATED_ROLE], $roles[BACKDROP_ANONYMOUS_ROLE]);
  }
  return $roles;
}

/**
 * Return the options lists for user status property.
 */
function entity_plus_metadata_user_status_options_list() {
  return array(
    0 => t('Blocked'),
    1 => t('Active'),
  );
}

/**
 * Callback defining an options list for language properties.
 */
function entity_plus_metadata_language_list() {
  $list = array();
  $list[LANGUAGE_NONE] = t('Language neutral');
  foreach (language_list() as $language) {
    $list[$language->langcode] = t($language->name);
  }
  return $list;
}

/**
 * Callback for getting field property values.
 */
function entity_plus_metadata_field_property_get($entity, array $options, $name, $entity_type, $info) {
  $field = field_info_field($name);
  $columns = array_keys($field['columns']);
  $langcode = isset($options['language']) ? $options['language']->langcode : LANGUAGE_NONE;
  $langcode = entity_plus_metadata_field_get_language($entity_type, $entity, $field, $langcode, TRUE);
  $values = array();
  if (isset($entity->{$name}[$langcode])) {
    foreach ($entity->{$name}[$langcode] as $delta => $data) {
      $values[$delta] = $data[$columns[0]];
      if ($info['type'] == 'boolean' || $info['type'] == 'list<boolean>') {
        // Ensure that we have a clean boolean data type.
        $values[$delta] = (bool) $values[$delta];
      }
    }
  }
  // For an empty single-valued field, we have to return NULL.
  return $field['cardinality'] == 1 ? ($values ? reset($values) : NULL) : $values;
}

/**
 * Callback for setting field property values.
 */
function entity_plus_metadata_field_property_set($entity, $name, $value, $langcode, $entity_type, $info) {
  $field = field_info_field($name);
  $columns = array_keys($field['columns']);
  $langcode = entity_plus_metadata_field_get_language($entity_type, $entity, $field, $langcode);
  $values = $field['cardinality'] == 1 ? array($value) : (array) $value;

  $items = array();
  foreach ($values as $delta => $value) {
    if (isset($value)) {
      $items[$delta][$columns[0]] = $value;
      if ($info['type'] == 'boolean' || $info['type'] == 'list<boolean>') {
        // Convert boolean values back to an integer for writing.
        $items[$delta][$columns[0]] = (int) $items[$delta][$columns[0]] = $value;
      }
    }
  }
  $entity->{$name}[$langcode] = $items;
  // Empty the static field language cache, so the field system picks up any
  // possible new languages.
  backdrop_static_reset('field_language');
}

/**
 * Callback returning the options list of a field.
 */
function entity_plus_metadata_field_options_list($name, $info) {
  $field_property_info = $info;
  if (is_numeric($name) && isset($info['parent'])) {
    // The options list is to be returned for a single item of a multiple field.
    $field_property_info = $info['parent']->info();
    $name = $field_property_info['name'];
  }
  if (($field = field_info_field($name)) && isset($field_property_info['parent'])) {
    // Retrieve the wrapped entity holding the field.
    $wrapper = $field_property_info['parent'];
    try {
      $entity = $wrapper->value();
    }
    catch (EntityMetadataWrapperException $e) {
      // No data available.
      $entity = NULL;
    }

    // Support translating labels via i18n field.
    if (module_exists('i18n_field') && ($translate = i18n_field_type_info($field['type'], 'translate_options'))) {
      return $translate($field);
    }
    else {
      $instance = $wrapper->getBundle() ? field_info_instance($wrapper->type(), $name, $wrapper->getBundle()) : NULL;
      return (array) module_invoke($field['module'], 'options_list', $field, $instance, $wrapper->type(), $entity);
    }
  }
}

/**
 * Callback to verbatim get the data structure of a field. Useful for fields that add metadata for their own data structure.
 */
function entity_plus_metadata_field_verbatim_get($entity, array $options, $name, $entity_type, &$context) {
  // Set contextual info useful for getters of any child properties.
  $context['instance'] = field_info_instance($context['parent']->type(), $name, $context['parent']->getBundle());
  $context['field'] = field_info_field($name);
  $langcode = isset($options['language']) ? $options['language']->langcode : LANGUAGE_NONE;
  $langcode = entity_plus_metadata_field_get_language($entity_type, $entity, $context['field'], $langcode, TRUE);

  if ($context['field']['cardinality'] == 1) {
    return isset($entity->{$name}[$langcode][0]) ? $entity->{$name}[$langcode][0] : NULL;
  }
  return isset($entity->{$name}[$langcode]) ? $entity->{$name}[$langcode] : array();
}

/**
 * Writes the passed field items in the object. Useful as field level setter to set the whole data structure at once.
 */
function entity_plus_metadata_field_verbatim_set($entity, $name, $items, $langcode, $entity_type) {
  $field = field_info_field($name);
  $langcode = entity_plus_metadata_field_get_language($entity_type, $entity, $field, $langcode);
  $value = $field['cardinality'] == 1 ? array($items) : (array) $items;
  // Filter out any items set to NULL.
  $entity->{$name}[$langcode] = array_filter($value);

  // Empty the static field language cache, so the field system picks up any
  // possible new languages.
  backdrop_static_reset('field_language');
}

/**
 * Helper for determining the field language to be used.
 *
 * Note that we cannot use field_language() as we are not about to display
 * values, but generally read/write values.
 *
 * @param string $entity_type
 *   The entity type; e.g., 'node' or 'user'.
 * @param Entity $entity
 *   The entity to which the field is attached.
 * @param array $field
 *   The field info as returned by field_info_field().
 * @param string $langcode
 *   (optional) The language code.
 * @param bool $fallback
 *   (optional) Whether to fall back to the entity default language, if no
 *   value is available for the given language code yet.
 *
 * @return string
 *   The language code to use.
 */
function entity_plus_metadata_field_get_language($entity_type, $entity, $field, $langcode = LANGUAGE_NONE, $fallback = FALSE) {
  $default_langcode = entity_plus_language($entity_type, $entity);

  // Determine the right language to use.
  if ($default_langcode != LANGUAGE_NONE && field_is_translatable($entity_type, $field)) {
    $langcode = ($langcode != LANGUAGE_NONE) ? field_valid_language($langcode, $default_langcode) : $default_langcode;
    if (!isset($entity->{$field['field_name']}[$langcode]) && $fallback) {
      $langcode = $default_langcode;
    }
    return $langcode;
  }
  else {
    return LANGUAGE_NONE;
  }
}

/**
 * Callback for getting the sanitized text of 'text_formatted' properties. This callback is used for both the 'value' and the 'summary'.
 */
function entity_plus_metadata_field_text_get($item, array $options, $name, $type, $context) {
  // $name is either 'value' or 'summary'.
  if (!isset($item['safe_' . $name])) {
    // Apply input formats.
    $langcode = isset($options['language']) ? $options['language']->langcode : LANGUAGE_NONE;
    $format = isset($item['format']) ? $item['format'] : filter_default_format();
    $item['safe_' . $name] = check_markup($item[$name], $format, $langcode);
    // To speed up subsequent calls, update $item with the 'safe_value'.
    $context['parent']->set($item);
  }
  return $item['safe_' . $name];
}

/**
 * Defines the list of all available text formats.
 */
function entity_plus_metadata_field_text_formats() {
  foreach (filter_formats() as $key => $format) {
    $formats[$key] = $format->name;
  }
  return $formats;
}

/**
 * Callback for getting the file entity of file fields.
 */
function entity_plus_metadata_field_file_get($item) {
  return $item['fid'];
}

/**
 * Callback for setting the file entity of file fields.
 */
function entity_plus_metadata_field_file_set(&$item, $property_name, $value) {
  $item['fid'] = $value;
}

/**
 * Callback for auto-creating file field $items.
 */
function entity_plus_metadata_field_file_create_item($property_name, $context) {
  // 'fid' is required, so 'file' has to be set as initial property.
  return array('display' => isset($context['field']['settings']['display_default']) ? $context['field']['settings']['display_default'] : 0);
}

/**
 * Callback for validating file field $items.
 */
function entity_plus_metadata_field_file_validate_item($items, $context) {
  // Allow NULL values.
  if (!isset($items)) {
    return TRUE;
  }

  // Stream-line $items for multiple vs non-multiple fields.
  $items = !entity_plus_property_list_extract_type($context['type']) ? array($items) : (array) $items;

  foreach ($items as $item) {
    // File-field items require a valid file.
    if (!isset($item['fid']) || !file_load($item['fid'])) {
      return FALSE;
    }
    if (isset($context['property info']['display']) && !isset($item['display'])) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * Access callback for the node entity.
 *
 * This function does not implement hook_node_access(), thus it may not be
 * called entity_plus_metadata_node_access().
 *
 * @see entity_plus_access()
 *
 * @param string $op
 *   The operation being performed. One of 'view', 'update', 'create' or
 *   'delete'.
 * @param Node $node
 *   A node to check access for. Must be a node object. Must have nid,
 *   except in the case of 'create' operations.
 * @param object $account
 *   The user to check for. Leave it to NULL to check for the global user.
 *
 * @throws EntityMalformedException
 *
 * @return bool
 *   TRUE if access is allowed, FALSE otherwise.
 */
function entity_plus_metadata_no_hook_node_access($op, $node = NULL, $account = NULL) {
  // First deal with the case where a $node is provided.
  if (isset($node)) {
    if ($op == 'create') {
      if (isset($node->type)) {
        return node_access($op, $node->type, $account);
      }
      else {
        throw new EntityMalformedException(t('Permission to create a node was requested but no node type was given.'));
      }
    }
    // If a non-default revision is given, incorporate revision access.
    $default_revision = node_load($node->nid);
    if ($node->vid !== $default_revision->vid) {
      return _node_revision_access($node, $op, $account);
    }
    else {
      return node_access($op, $node, $account);
    }
  }
  // No node is provided. Check for access to all nodes.
  if (user_access('bypass node access', $account)) {
    return TRUE;
  }
  if (!user_access('access content', $account)) {
    return FALSE;
  }
  if ($op == 'view' && node_access_view_all_nodes($account)) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Access callback for the user entity.
 */
function entity_plus_metadata_user_access($op, $entity = NULL, $account = NULL, $entity_type = NULL) {
  $account = isset($account) ? $account : $GLOBALS['user'];
  // Grant access to the users own user account and to the anonymous one.
  if (isset($entity->uid) && $op != 'delete' && (($entity->uid == $account->uid && $entity->uid) || (!$entity->uid && $op == 'view'))) {
    return TRUE;
  }
  if (user_access('administer users', $account)
    || user_access('access user profiles', $account) && $op == 'view' && (empty($entity) || !empty($entity->status))) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Access callback for restricted user properties.
 */
function entity_plus_metadata_user_properties_access($op, $property, $entity = NULL, $account = NULL) {
  if (user_access('administer users', $account)) {
    return TRUE;
  }
  $account = isset($account) ? $account : $GLOBALS['user'];
  // Flag to indicate if this user entity is the own user account.
  $is_own_account = isset($entity->uid) && $account->uid == $entity->uid;
  switch ($property) {
    case 'name':
      // Allow view access to anyone with access to the entity.
      if ($op == 'view') {
        return TRUE;
      }
      // Allow edit access for own user name if the permission is satisfied.
      return $is_own_account && user_access('change own username', $account);

    case 'mail':
      // Allow access to own mail address.
      return $is_own_account;

    case 'roles':
      // Allow view access for own roles.
      return ($op == 'view' && $is_own_account);
  }
  return FALSE;
}

/**
 * Access callback for the comment entity.
 */
function entity_plus_metadata_comment_access($op, $entity = NULL, $account = NULL) {
  // When determining access to a comment, 'comment_access' does not take any
  // access restrictions to the comment's associated node into account. If a
  // comment has an associated node, the user must be able to view it in order
  // to access the comment.
  if (isset($entity->nid)) {
    if (!entity_plus_access('view', 'node', node_load($entity->nid), $account)) {
      return FALSE;
    }
  }

  // Comment administrators are allowed to perform all operations on all
  // comments.
  if (user_access('administer comments', $account)) {
    return TRUE;
  }

  // Unpublished comments can never be accessed by non-admins.
  if (isset($entity->status) && $entity->status == COMMENT_NOT_PUBLISHED) {
    return FALSE;
  }

  if (isset($entity) && $op == 'update') {
    // Because 'comment_access' only checks the current user, we need to do our
    // own access checking if an account was specified.
    if (!isset($account)) {
      return comment_access('edit', $entity);
    }
    else {
      return $account->uid && $account->uid == $entity->uid && user_access('edit own comments', $account);
    }
  }
  if (user_access('access comments', $account) && $op == 'view') {
    return TRUE;
  }
  return FALSE;
}

/**
 * Access callback for restricted comment properties.
 */
function entity_plus_metadata_comment_properties_access($op, $property, $entity = NULL, $account = NULL) {
  return user_access('administer comments', $account);
}

/**
 * Access callback for the taxonomy entities.
 */
function entity_plus_metadata_taxonomy_access($op, $entity = NULL, $account = NULL, $entity_type = NULL) {
  if (isset($entity) && $op == 'update' && !isset($account) && user_access("edit terms in {$entity->vocabulary}")) {
    return TRUE;
  }
  if (user_access('administer taxonomy', $account) || user_access('access content', $account) && $op == 'view') {
    return TRUE;
  }
  return FALSE;
}

/**
 * Access callback for file entities.
 *
 * @see entity_plus_access()
 *
 * @param string $op
 *   The operation being performed. One of 'view', 'download', 'update', 'create' or
 *   'delete'.
 * @param File $file
 *   A file to check access for. Must be a file object. Must have fid,
 *   except in the case of 'create' operations.
 * @param User $account
 *   The user to check for. Leave it to NULL to check for the global user.
 *
 * @throws EntityMalformedException
 *
 * @return bool
 *   TRUE if access is allowed, FALSE otherwise.
 */
function entity_plus_metadata_file_access($op, $file = NULL, $account = NULL) {
  // First deal with the case where a $file is provided and op is 'create'.
  if (isset($file) && $op == 'create') {
    if (isset($file->type)) {
      return file_access($op, $file->type, $account);
    }
    else {
      throw new EntityMalformedException(t('Permission to create a file was requested but no file type was given.'));
    }
  }
  // No file is provided. Check for access to all files.
  if (file_access($op, $file, $account)) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Callback to determine access for properties which are fields.
 */
function entity_plus_metadata_field_access_callback($op, $name, $entity = NULL, $account = NULL, $entity_type = NULL) {
  $field = field_info_field($name);
  return field_access($op, $field, $entity_type, $entity, $account);
}

/**
 * Callback to create entity objects.
 */
function entity_plus_metadata_create_object($values = array(), $entity_type = NULL) {
  $info = entity_get_info($entity_type);
  // Make sure at least the bundle and label properties are set.
  if (isset($info['entity keys']['bundle']) && $key = $info['entity keys']['bundle']) {
    $values += array($key => NULL);
  }
  if (isset($info['entity keys']['label']) && $key = $info['entity keys']['label']) {
    $values += array($key => NULL);
  }
  $entity = (object) $values;
  $entity->is_new = TRUE;
  return $entity;
}

/**
 * Callback to create a new comment.
 */
function entity_plus_metadata_create_comment($values = array()) {
  $comment = (object) ($values + array(
    'status' => COMMENT_PUBLISHED,
    'pid' => 0,
    'subject' => '',
    'uid' => 0,
    'langcode' => LANGUAGE_NONE,
    'node_type' => NULL,
    'is_new' => TRUE,
  ));
  $comment->cid = FALSE;
  return $comment;
}

/**
 * Callback to create a new node.
 */
function entity_plus_metadata_create_node($values = array()) {
  $node = (object) array(
    'type' => $values['type'],
    'langcode' => LANGUAGE_NONE,
    'is_new' => TRUE,
  );
  // Set some defaults.
  $node_options_settings = config_get('node.type.' . $node->type, 'settings');
  foreach (array('status', 'promote', 'sticky') as $key) {
    $node->$key = (int) $node_options_settings[$key . '_default'];
  }
  if (module_exists('comment') && !isset($node->comment)) {
    $node->comment = !empty($node_options_settings['comment_default']) ? $node_options_settings['comment_default'] : COMMENT_NODE_OPEN;
  }
  // Apply the given values.
  foreach ($values as $key => $value) {
    $node->$key = $value;
  }
  return $node;
}

/**
 * Callback to delete a file. Watch out to not accidentilly implement hook_file_delete().
 */
function entity_plus_metadata_delete_file($fid) {
  file_delete(file_load($fid), TRUE);
}

/**
 * Callback to view nodes.
 */
function entity_plus_metadata_view_node($entities, $view_mode = 'full', $langcode = NULL) {
  $result = node_view_multiple($entities, $view_mode, 0, $langcode);
  // Make sure to key the result with 'node' instead of 'nodes'.
  return array('node' => reset($result));
}

/**
 * Callback to view comments.
 */
function entity_plus_metadata_view_comment($entities, $view_mode = 'full', $langcode = NULL) {
  $build = array();
  $nodes = array();
  // The comments, indexed by nid and then by cid.
  $nid_comments = array();
  foreach ($entities as $cid => $comment) {
    $nid = $comment->nid;
    $nodes[$nid] = $nid;
    $nid_comments[$nid][$cid] = $comment;
  }
  $nodes = node_load_multiple(array_keys($nodes));
  foreach ($nid_comments as $nid => $comments) {
    $node = isset($nodes[$nid]) ? $nodes[$nid] : NULL;
    $build += comment_view_multiple($comments, $node, $view_mode, 0, $langcode);
  }
  return array('comment' => $build);
}

/**
 * Callback to view an entity, for which just ENTITYTYPE_view() is available.
 */
function entity_plus_metadata_view_single($entities, $view_mode = 'full', $langcode = NULL, $entity_type = NULL) {
  $function = $entity_type . '_view';
  $build = array();
  foreach ($entities as $key => $entity) {
    $build[$entity_type][$key] = $function($entity, $view_mode, $langcode);
  }
  return $build;
}

/**
 * Callback to get the form of a node.
 */
function entity_plus_metadata_form_node($node) {
  // Pre-populate the form-state with the right form include.
  $form_state['build_info']['args'] = array($node);
  form_load_include($form_state, 'inc', 'node', 'node.pages');
  return backdrop_build_form($node->type . '_node_form', $form_state);
}

/**
 * Callback to get the form of a comment.
 */
function entity_plus_metadata_form_comment($comment) {
  if (!isset($comment->node_type)) {
    $node = node_load($comment->nid);
    $comment->node_type = 'comment_node_' . $node->type;
  }
  return backdrop_get_form($comment->node_type . '_form', $comment);
}

/**
 * Callback to get the form of a user account.
 */
function entity_plus_metadata_form_user($account) {
  // If $account->uid is set then we want a user edit form.
  // Otherwise we want the user register form.
  if (isset($account->uid)) {
    $form_id = 'user_profile_form';
    form_load_include($form_state, 'inc', 'user', 'user.pages');
  }
  else {
    $form_id = 'user_register_form';
  }
  $form_state['build_info']['args'] = array($account);
  return backdrop_build_form($form_id, $form_state);
}

/**
 * Callback to get the form of a term.
 */
function entity_plus_metadata_form_taxonomy_term($term) {
  // Pre-populate the form-state with the right form include.
  $form_state['build_info']['args'] = array($term);
  form_load_include($form_state, 'inc', 'taxonomy', 'taxonomy.admin');
  return backdrop_build_form('taxonomy_form_term', $form_state);
}

/**
 * Callback to get the form of a vocabulary.
 */
function entity_plus_metadata_form_taxonomy_vocabulary($vocab) {
  // Pre-populate the form-state with the right form include.
  $form_state['build_info']['args'] = array($vocab);
  form_load_include($form_state, 'inc', 'taxonomy', 'taxonomy.admin');
  return backdrop_build_form('taxonomy_form_vocabulary', $form_state);
}

/**
 * Callback to get the form for entities using the entity API admin ui.
 */
function entity_plus_metadata_form_entity_plus_ui($entity, $entity_type) {
  $info = entity_get_info($entity_type);
  $form_state = form_state_defaults();
  // Add in the include file as the form API does else with the include file
  // specified for the active menu item.
  if (!empty($info['admin ui']['file'])) {
    $path = isset($info['admin ui']['file path']) ? $info['admin ui']['file path'] : backdrop_get_path('module', $info['module']);
    $form_state['build_info']['files']['entity_plus_ui'] = $path . '/' . $info['admin ui']['file'];
    // Also load the include file.
    if (file_exists($form_state['build_info']['files']['entity_plus_ui'])) {
      require_once BACKDROP_ROOT . '/' . $form_state['build_info']['files']['entity_plus_ui'];
    }
  }
  return entity_plus_ui_get_form($entity_type, $entity, $op = 'edit', $form_state);
}

/**
 * Callback for querying entity properties having their values stored in the entities main db table.
 */
function entity_plus_metadata_table_query($entity_type, $property, $value, $limit) {
  $properties = entity_plus_get_all_property_info($entity_type);
  $info = $properties[$property] + array('schema field' => $property);

  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', $entity_type, '=')
        ->propertyCondition($info['schema field'], $value, is_array($value) ? 'IN' : '=')
        ->range(0, $limit);

  $result = $query->execute();
  return !empty($result[$entity_type]) ? array_keys($result[$entity_type]) : array();
}

/**
 * Callback for querying entities by field values. This function just queries for the value of the first specified column. Also it is only suitable for fields that don't process the data, so it's stored the same way as returned.
 */
function entity_plus_metadata_field_query($entity_type, $property, $value, $limit) {
  $query = new EntityFieldQuery();
  $field = field_info_field($property);
  $columns = array_keys($field['columns']);

  $query->entityCondition('entity_type', $entity_type, '=')
        ->fieldCondition($field, $columns[0], $value, is_array($value) ? 'IN' : '=')
        ->range(0, $limit);

  $result = $query->execute();
  return !empty($result[$entity_type]) ? array_keys($result[$entity_type]) : array();
}

/**
 * Implements entity_uri() callback for file entities.
 */
function entity_plus_metadata_uri_file($file) {
  return array(
    'path' => file_create_url($file->uri),
  );
}

/**
 * Getter callback to return date values as datestamp in UTC from the field.
 */
function entity_plus_metadata_field_date_getter($entity, array $options, $name, $entity_type, &$context) {
  $return = entity_plus_metadata_field_verbatim_get($entity, $options, $name, $entity_type, $context);
  $items = ($context['field']['cardinality'] == 1) ? array($return) : $return;
  foreach ($items as $key => $item) {
    $items[$key] = entity_plus_metadata_date_struct_getter($item, $options, 'value', 'struct', $context);
  }
  return ($context['field']['cardinality'] == 1) ? $items[0] : $items;
}

/**
 * Getter callback to return date values as datestamp in UTC.
 */
function entity_plus_metadata_date_struct_getter($item, array $options, $name, $type, $info) {
  if (empty($item) || !isset($item[$name])) {
    return NULL;
  }
  $value = trim($item[$name]);
  if (empty($value)) {
    return NULL;
  }

  $timezone_db = !empty($item['timezone_db']) ? $item['timezone_db'] : 'UTC';
  $date = new BackdropDateTime($value, $timezone_db);
  return !empty($date) ? date_format_date($date, 'custom', 'U') : NULL;
}

/**
 * Getter callback to return the duration of the time period given by the dates.
 */
function entity_plus_metadata_date_duration_getter($item, array $options, $name, $type, $info) {
  $value = entity_plus_metadata_date_struct_getter($item, $options, 'value', 'struct', $info);
  $value2 = entity_plus_metadata_date_struct_getter($item, $options, 'value2', 'struct', $info);
  if ($value && $value2) {
    return $value2 - $value;
  }
}

/**
 * Callback for setting field property values.
 *
 * Based on entity_metadata_field_property_set(), the original property setter,
 * adapted to transform non-timestamp date values to timestamps.
 */
function entity_plus_metadata_field_date_setter(&$entity, $name, $value, $langcode, $entity_type, $info) {
  $field = field_info_field($name);
  if (!isset($langcode)) {
    // Try to figure out the default language used by the entity.
    // @todo: Update once http://backdrop.org/node/1260640 has been fixed.
    $langcode = isset($entity->language) ? $entity->language : LANGUAGE_NONE;
  }
  $values = $field['cardinality'] == 1 ? array($value) : (array) $value;

  $items = array();
  foreach ($values as $delta => $value) {
    // Make use of the struct setter to convert the date back to a timestamp.
    $info['field_name'] = $name;
    entity_plus_metadata_date_struct_setter($items[$delta], 'value', $value, $langcode, 'struct', $info);
  }
  $entity->{$name}[$langcode] = $items;
  // Empty the static field language cache, so the field system picks up any
  // possible new languages.
  backdrop_static_reset('field_language');
}

/**
 * Auto creation callback for fields which contain two date values in one.
 */
function entity_plus_metadata_date_struct_create($name, $property_info) {
  return array(
    'date_type' => $property_info['field']['columns'][$name]['type'],
    'timezone_db' => $property_info['field']['settings']['timezone_db'],
  );
}

/**
 * Callback for setting an individual field value if a to-date may be there too.
 *
 * Based on entity_property_verbatim_set().
 *
 * The passed in unix timestamp (UTC) is converted to the right value and format dependent on the field.
 *
 * $name is either 'value' or 'value2'.
 */
function entity_plus_metadata_date_struct_setter(&$item, $name, $value, $langcode, $type, $info) {
  if (!isset($value)) {
    $item[$name] = NULL;
  }
  else {
    $field = field_info_field($info['field_name']);
    $format = date_type_format($field['type']);
    $timezone_db = date_get_timezone_db($field['settings']['tz_handling']);

    $date = new BackdropDateTime($value, 'UTC');
    if ($timezone_db != 'UTC') {
      date_timezone_set($date, timezone_open($timezone_db));
    }
    $item[$name] = $date->format($format);
  }
}

/**
 * Callback for creating a new, empty link field item.
 *
 * @see entity_plus_metadata_field_link_callback()
 */
function entity_plus_metadata_field_link_create_item() {
  return array('title' => NULL, 'url' => NULL, 'display_url' => NULL);
}


/**
 * Callback for getting the link URL property.
 */
function entity_plus_link_url_property_get($data, array $options, $name, $type, $info) {
  $url = entity_plus_property_verbatim_get($data, $options, $name, $type, $info);

  return url($url, $data + $options);
}

/**
 * Entity property info getter callback for link attributes.
 */
function entity_plus_link_attribute_property_get($data, array $options, $name, $type, $info) {
  return isset($data[$name]) ? array_filter($data[$name]) : array();
}
