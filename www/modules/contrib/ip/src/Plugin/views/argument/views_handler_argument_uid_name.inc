<?php
/**
 * @file
 * Views argument handler for User ID-based name information.
 */

class views_handler_argument_uid_name extends views_handler_argument {

  /**
   * Overrides views_handler::option_definition().
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['statuses'] = array(
      'default' => 'cancelled',
      'translatable' => TRUE,
    );
    return $options;
  }

  /**
   * Overrides views_handler::options_form().
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['statuses'] = array(
      '#title' => t('Status argument values'),
      '#description' => t('Select the argument values that will be accepted. Multiple values can be submitted by combining with "+", e.g., "cancelled+active".'),
      '#type' => 'checkboxes',
      '#options' => array(
        'cancelled' => t('cancelled'),
        'active' => t('active'),
        'blocked' => t('blocked'),
        'anonymous' => t('anonymous'),
      ),
      '#default_value' => $this->options['statuses']
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
    $arguments = explode('+', $this->argument);
    $conditions = array();
    foreach ($arguments as $value) {
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
