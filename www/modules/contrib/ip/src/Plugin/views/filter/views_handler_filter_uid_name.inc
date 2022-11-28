<?php
/**
 * @file
 * Views filter handler for User ID-based name information.
 */

class views_handler_filter_uid_name extends views_handler_filter_string {

  /**
   * Overrides views_handler_filter::query().
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
    $table = $this->query->add_relationship($this->table_alias . '__users', $join, 'users');
    $field = "$table.name";
    $info = $this->operators();
    if (!empty($info[$this->operator]['method'])) {
      $this->{$info[$this->operator]['method']}($field);
    }
  }
}
