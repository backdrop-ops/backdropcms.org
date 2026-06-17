<?php

/**
 * Views query class using a Search API index as the data source.
 */
class SearchApiMultiViewsQuery extends SearchApiViewsQuery {

  /**
   * The query that will be executed.
   *
   * @var SearchApiMultiQueryInterface
   */
  protected $query;

  /**
   * Create the basic query object and fill with default values.
   */
  public function init($base_table, $base_field, $options) {
    try {
      parent::init($base_table, $base_field, $options);
      $this->query = search_api_multi_query(array('parse mode' => $this->options['parse_mode']));
    }
    catch (Exception $e) {
      $this->errors[] = $e->getMessage();
    }
  }

  /**
   * Adds settings for the UI.
   *
   * Adds an option for bypassing access checks.
   */
  public function options_form(&$form, &$form_state) {
    $form['search_api_bypass_access'] = array(
      '#type' => 'checkbox',
      '#title' => t('Bypass access checks'),
      '#description' => t('If the underlying search index has access checks enabled, this option allows to disable them for this view.'),
      '#default_value' => $this->options['search_api_bypass_access'],
    );

    $form['entity_access'] = array(
      '#type' => 'checkbox',
      '#title' => t('Additional access checks on result entities'),
      '#description' => t("Execute an access check for all result entities. This prevents users from seeing inappropriate content when the index contains stale data, or doesn't provide access checks. However, result counts, paging and other things won't work correctly if results are eliminated in this way, so only use this as a last ressort (and in addition to other checks, if possible)."),
      '#default_value' => $this->options['entity_access'],
    );

    $form['parse_mode'] = array(
      '#type' => 'select',
      '#title' => t('Parse mode'),
      '#description' => t('Choose how the search keys will be parsed.'),
      '#options' => array(),
      '#default_value' => $this->options['parse_mode'],
    );
    foreach ($this->query->parseModes() as $key => $mode) {
      $form['parse_mode']['#options'][$key] = $mode['name'];
      if (!empty($mode['description'])) {
        $states['visible'][':input[name="query[options][parse_mode]"]']['value'] = $key;
        $form["parse_mode_{$key}_description"] = array(
          '#type' => 'item',
          '#title' => $mode['name'],
          '#description' => $mode['description'],
          '#states' => $states,
        );
      }
    }
  }

  /**
   * Helper function for adding results to a view in the format expected by the
   * view.
   */
  protected function addResults(array $results, $view) {
    $indexes = $this->getIndexes();
    foreach ($results as $result) {
      $row = array();

      if (!empty($this->options['entity_access']) && isset($indexes[$result['index_id']]) && $indexes[$result['index_id']]->getEntityType()) {
        $entity = $indexes[$result['index_id']]->loadItems(array($result['id']));
        if (!$entity || !entity_access('view', $indexes[$result['index_id']]->item_type, reset($entity))) {
          continue;
        }
      }
      // Include the loaded item for this result row, if present, or the item
      // ID.
      if (!empty($result['entity'])) {
        $row['entity'] = $result['entity'];
      }
      else {
        $row['entity'] = $result['id'];
      }

      // Gather any fields from the search results.
      if (!empty($result['fields'])) {
        foreach (search_api_get_sanitized_field_values($result['fields']) as $field_id => $value) {
          $field_id = $result['index_id'] . ':' . $field_id;
          $row['_entity_properties'][$field_id] = $value;
        }
      }

      $row['_entity_properties']['search_api_multi_id'] = $result['id'];
      $row['_entity_properties']['search_api_multi_index'] = $result['index_id'];
      $row['_entity_properties']['search_api_relevance'] = $result['score'];
      $row['_entity_properties']['search_api_excerpt'] = empty($result['excerpt']) ? '' : $result['excerpt'];

      // Add the row to the Views result set.
      $view->result[] = (object) $row;
    }
  }

  /**
   * Returns the according metadata wrappers for the given query results.
   *
   * This is necessary to support generic entity handlers and plugins with this
   * query backend.
   */
  public function get_result_wrappers($results, $relationship = NULL, $field = NULL) {
    $wrappers = array();
    $load_items = array();
    $entity_types = entity_get_info();
    $indexes = $this->getIndexes();

    // Entity property info for the results.
    $info = array();
    foreach ($indexes as $index_id => $index) {
      $entity_type = $index->getEntityType();
      $info['property info'][$index_id] = array(
        'label' => t('@index results', array('@index' => $index->name)),
        'type' => $entity_type ? $entity_type : $index->item_type,
        'description' => t('Results from the @index index.', array('@index' => $index->name)),
      );
      if (!$entity_type) {
        $info['property info'][$index_id] += $index->entityWrapper()->info();
      }
    }

    // Pick out all results that need to be loaded.
    foreach ($results as $row_index => $row) {
      $index_id = $row->_entity_properties['search_api_multi_index'];
      if (isset($row->entity) && !empty($indexes[$index_id])) {
        $index = $indexes[$index_id];
        // If this item isn't loaded, register it for pre-loading.
        if (is_scalar($row->entity)) {
          $load_items[$index->item_type][$row->entity] = $row->entity;
        }
      }
    }

    // Load the results in bulk, by item type, and create the wrappers.
    if (!empty($load_items)) {
      foreach ($load_items as $type => $ids) {
        try {
          $load_items[$type] = search_api_get_datasource_controller($type)->loadItems($ids);
        }
        catch (SearchApiException $e) {
          watchdog_exception('search_api_multi', $e);
        }
      }
    }

    // Create wrappers for all results.
    foreach ($results as $row_index => $row) {
      $index_id = $row->_entity_properties['search_api_multi_index'];
      if ($indexes[$index_id]) {
        if (is_scalar($row->entity)) {
          $index = $indexes[$index_id];
          if (empty($load_items[$index->item_type][$row->entity])) {
            continue;
          }
          $row->entity = $load_items[$index->item_type][$row->entity];
        }
        $item = $row->entity;
        $data = new stdClass();
        $data->{$index_id} = $item;
        $wrappers[$row_index] = entity_metadata_wrapper('search_api_multi', $data, $info);
      }
    }

    // Apply the relationship, if necessary.
    $type = 'search_api_multi';
    $selector_suffix = '';
    if ($field && ($pos = strrpos($field, ':'))) {
      $selector_suffix = substr($field, 0, $pos);
    }
    if ($selector_suffix || ($relationship && !empty($this->view->relationship[$relationship]))) {
      // Use EntityPlusFieldHandlerHelper to compute the correct data selector
      // for the relationship.
      $handler = (object) array(
        'view' => $this->view,
        'relationship' => $relationship,
        'real_field' => '',
      );
      $selector = EntityPlusFieldHandlerHelper::construct_property_selector($handler);
      $selector .= ($selector ? ':' : '') . $selector_suffix;
      list($type, $wrappers) = EntityPlusFieldHandlerHelper::extract_property_multiple($wrappers, $selector);
    }

    return array($type, $wrappers);
  }

  //
  // Query interface methods (proxy to $this->query)
  //

  /**
   * Retrieves the searched indexes.
   *
   * @return SearchApiIndex[]
   *   An array of SearchApiIndex objects representing all indexes that will be
   *   used for this search, keyed by machine names.
   */
  public function getIndexes() {
    if (!$this->errors) {
      return $this->query->getIndexes();
    }
    return array();
  }

}
