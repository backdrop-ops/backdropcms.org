<?php
/**
 * @file
 * Definition of views_handler_sort_uid_status.
 */

/**
 * Handler for views_handler_sort_ud_status, sort by status based on uid
 *
 * @ingroup views_sort_handlers
 * @see views_handler_field_uid_status
 */
class views_handler_sort_uid_status extends views_handler_sort {

  /**
   * Overrides views_handler_sort::query().
   */
  public function query() {
    $this->ensure_my_table();
    $join = new views_join();
    $join->definition = array(
      'left_table' => $this->table_alias,
      'left_field' => 'uid',
      'table' => 'users',
      'field' => 'uid',
      'type' => 'LEFT',
    );
    $join->construct();
    $table_alias = $this->query->add_relationship($this->table_alias . '__users', $join, 'users');
    $this->uid_status_uid_alias = $this->query->add_field($table_alias, 'uid');
    $this->uid_status_status_alias = $this->query->add_field($table_alias, 'status');



    $this->query->add_orderby($table_alias, NULL, $this->options['order'], $this->uid_status_status_alias);
  }
}
