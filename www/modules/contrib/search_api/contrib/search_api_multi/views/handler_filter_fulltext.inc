<?php

/**
 * Views filter handler class for handling fulltext fields.
 */
class SearchApiMultiHandlerFilterFulltext extends SearchApiViewsHandlerFilterFulltext {


  /**
 *
 */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['fields']['#description'] = t('Select the fields that will be searched. If no fields are selected and the "Search keys" mode is used, all available fulltext fields will be searched.');
  }

  /**
   * Overrides SearchApiViewsHandlerFilterFulltext::query().
   *
   * Sets the fields to NULL if none are selected. This prevents errors when not
   * all available indexes are searched.
   */
  public function query() {
    while (is_array($this->value)) {
      $this->value = $this->value ? reset($this->value) : '';
    }
    // Catch empty strings entered by the user, but not "0".
    if ($this->value === '') {
      return;
    }
    $fields = $this->options['fields'];

    // If something already specifically set different fields, we silently fall
    // back to mere filtering.
    $filter = $this->options['mode'] == 'filter';
    if (!$filter) {
      $old = $this->query->getFields();
      $filter = $old && $fields && (array_diff($old, $fields) || array_diff($fields, $old));
    }

    if ($filter) {
      $filter = $this->query->createFilter('OR');
      foreach ($fields as $field) {
        $filter->condition($field, $this->value, $this->operator);
      }
      $this->query->filter($filter);
      return;
    }

    // If the operator was set to OR, set it as the conjunction. (AND is set by
    // default.)
    if ($this->operator === 'OR') {
      $this->query->setOption('conjunction', $this->operator);
    }

    if ($fields) {
      $this->query->fields($fields);
    }
    $old = $this->query->getOriginalKeys();
    $this->query->keys($this->value);
    if ($this->operator == 'NOT') {
      $keys = &$this->query->getKeys();
      if (is_array($keys)) {
        $keys['#negation'] = TRUE;
      }
      else {
        // We can't know how negation is expressed in the server's syntax.
      }
    }
    if ($old) {
      $keys = &$this->query->getKeys();
      if (is_array($keys)) {
        $keys[] = $old;
      }
      elseif (is_array($old)) {
        // We don't support such nonsense.
      }
      else {
        $keys = "($old) ($keys)";
      }
    }
  }

  /**
   * Helper method to get an option list of all available fulltext fields.
   */
  protected function getFulltextFields() {
    $fields = array();
    $indexes = search_api_index_load_multiple(FALSE, array('enabled' => TRUE));
    foreach ($indexes as $index) {
      if ($index->getFields()) {
        $prefix = $index->machine_name . ':';
        $prefix_name = $index->name . ' » ';
        $f = $index->getFields();
        foreach ($index->getFulltextFields() as $name) {
          $fields[$prefix . $name] = $prefix_name . $f[$name]['name'];
        }
      }
    }
    return $fields;
  }

}
