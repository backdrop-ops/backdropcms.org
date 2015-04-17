<?php

/**
 * Views filter handler for matching project release nodes.
 */
class project_release_handler_filter_release_node_nid extends views_handler_filter {
  // exposed filter options
  var $no_single = TRUE;

  function construct() {
    $this->release_type = 'recommended';
    $this->release_type_options = array(
      'recommended' => t('Recommended'),
      'latest' => t('Latest'),
      'either' => t('Latest or recommended'),
    );
    $this->value_value = t('Latest/recommended release');
    if (isset($this->definition['label'])) {
      $this->value_value = $this->definition['label'];
    }
    parent::construct();
  }

  function admin_summary() {
    if (!empty($this->options['exposed'])) {
      return t('exposed');
    }
    return $this->release_type_options[$this->release_type];
  }

  /**
   * Provide simple equality operator
   */
  function operator_options() {
    return array(
      '=' => t('Is'),
      '!=' => t('Is not'),
    );
  }

  function value_form(&$form, &$form_state) {
    if (!empty($form_state['exposed'])) {
      // Exposed filter: use a select box to save space.
      $filter_form_type = 'select';
    }
    else {
      // Configuring a filter: use radios for clarity.
      $filter_form_type = 'radios';
    }
    $form['value'] = array(
      '#type' => $filter_form_type,
      '#title' => $this->value_value,
      '#options' => $this->release_type_options,
      '#default_value' => $this->release_type,
    );
    if (!empty($this->options['exposed'])) {
      $identifier = $this->options['expose']['identifier'];
      if (!isset($form_state['input'][$identifier])) {
        $form_state['input'][$identifier] = $this->value;
      }
      // If we're configuring an exposed filter, add an <Any> option.
      $any_label = config_get('views.settings', 'exposed_filter_any_label') == 'old_any' ? t('<Any>') : t('- Any -');
      if ($form['value']['#type'] != 'select') {
        $any_label = check_plain($any_label);
      }
      $form['value']['#options'] = array('All' => $any_label) + $form['value']['#options'];
    }
  }

  function value_validate($form, &$form_state) {
    if ($form_state['values']['options']['value'] == 'All' && empty($form_state['values']['options']['expose']['optional'])) {
      form_set_error('value', t('You must select a value unless this is an optional exposed filter.'));
    }
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['release_relationship'] = array('default' => 'base');
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $relationship_options['base'] = t('Base table');
    $relationships = $this->view->display_handler->get_option('relationships');
    foreach ($relationships as $relationship) {
      $relationship_handler = views_get_handler($relationship['table'], $relationship['field'], 'relationship');
      // Ignore invalid/broken relationships.
      if (empty($relationship_handler)) {
        continue;
      }
      $relationship_options[$relationship['id']] = $relationship_handler->label();
    }
    $form['release_relationship'] = array(
      '#type' => 'select',
      '#title' => t('Relationship for releases'),
      '#options' => $relationship_options,
      '#default_value' => $this->options['release_relationship'],
      '#description' => t('This filter only works if your view references release nodes. You must select which relationship in your view points to release nodes, or use %base_table if the view is directly querying release nodes.', array('%base_table' => t('Base table'))),
    );
  }

  /**
   * Build a query that restricts base table nodes that match our own field.
   */
  function query() {
    $this->ensure_my_table();

    if ($this->options['release_relationship'] == 'base') {
      $base_nid = $this->query->base_table . '.' . $this->query->base_field;
    }
    else {
      // We have to find the proper table alias and field name for the
      // relationship selected when the admin configured this filter.
      $relationship_id = $this->options['release_relationship'];
      $relationship = $this->view->relationship[$relationship_id];
      $table_alias = $relationship->alias;
      $relationship_field = !empty($relationship->definition['relationship field']) ? $relationship->definition['relationship field'] : $relationship->definition['real field'];
      $base_nid = "$table_alias.$relationship_field";
    }

    if ($this->value == 'either') {
      $recommended = $this->_where_clause('recommended', $base_nid);
      $latest = $this->_where_clause('latest', $base_nid);
      // If we're doing !=, we need to AND these to get 'neither'...
      $bool_op = $this->operator == '=' ? 'OR' : 'AND';
      $where = "($recommended $bool_op $latest)";
    }
    else {
      $where = $this->_where_clause($this->value, $base_nid);
    }
    $this->query->add_where_expression($this->options['group'], $where);
  }

  /**
   * Private helper to generate the WHERE clause for a given release type.
   */
  function _where_clause($release_type, $base_nid) {
    $where = $this->table_alias . '.';
    $where .= $release_type == 'latest' ? 'latest_release' : 'recommended_release';
    $where .= " $this->operator $base_nid";
    return $where;
  }
}
