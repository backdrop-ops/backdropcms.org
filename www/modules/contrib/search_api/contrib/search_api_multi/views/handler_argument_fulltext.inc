<?php

/**
 * Views argument handler class for handling fulltext fields.
 */
class SearchApiMultiHandlerArgumentFulltext extends SearchApiViewsHandlerArgumentFulltext {

  /**
   * Overrides SearchApiViewsHandlerArgumentFulltext::getFulltextFields().
   *
   * Provides the correct options in the "Fields" select list.
   */
  public function getFulltextFields() {
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
