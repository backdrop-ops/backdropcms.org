<?php
/**
 * @file
 * Contains the SearchApiMultiViewsHandlerFilterOptions class.
 */

/**
 * Views filter handler for fields with a limited set of possible values.
 */
class SearchApiMultiViewsHandlerFilterOptions extends SearchApiViewsHandlerFilterOptions {

  /**
   * {@inheritdoc}
   */
  protected function get_wrapper() {
    list($index_id, $property) = explode(':', $this->real_field, 2);
    $index = search_api_index_load($index_id);
    if (!$index) {
      return NULL;
    }
    $wrapper = $index->entityWrapper(NULL, TRUE);
    $parts = explode(':', $property);
    foreach ($parts as $i => $part) {
      if (!isset($wrapper->$part)) {
        return NULL;
      }
      $wrapper = $wrapper->$part;
      $info = $wrapper->info();
      if ($i < count($parts) - 1) {
        // Unwrap lists.
        $level = search_api_list_nesting_level($info['type']);
        for ($j = 0; $j < $level; ++$j) {
          $wrapper = $wrapper[0];
        }
      }
    }

    return $wrapper;
  }

  /**
   * {@inheritdoc}
   */
  protected function get_value_options() {
    if ($this->real_field == 'search_api_multi_index') {
      $this->value_options = array();
      foreach (search_api_index_load_multiple(FALSE, array('enabled' => 1)) as $index) {
        $this->value_options[$index->machine_name] = check_plain($index->name);
      }
    }
    else {
      parent::get_value_options();
    }
  }

}
