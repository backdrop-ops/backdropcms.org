<?php
/**
 * @file
 * Views filter handler for User ID status information.
 */

class views_handler_filter_uid_status extends views_handler_filter_in_operator {

  /**
   * Overrides views_handler_filter_in_operator::get_value_options().
   */
  function get_value_options() {
    $this->value_options = array(
      'cancelled' => t('Cancelled'),
      'active' => t('Active'),
      'blocked' => t('Blocked'),
      'anonymous' => t('Anonymous'),
    );
  }

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
    $conditions = array();
    foreach ($this->value as $value) {
      if ($value == 'cancelled') {
        $conditions[] = "($table.uid IS NULL)";
      }
      elseif ($value == 'active') {
        $conditions[] = "($table.status = 1)";
      }
      elseif ($value == 'blocked') {
        $conditions[] = "(($table.status = 0) AND ($table.uid <> 0))";
      }
      elseif ($value == 'anonymous') {
        $conditions[] = "($table.uid = 0)";
      }
    }
    if (!empty($conditions)) {
      $this->query->add_where_expression($this->options['group'], implode(' OR ', $conditions));
    }
  }
}
