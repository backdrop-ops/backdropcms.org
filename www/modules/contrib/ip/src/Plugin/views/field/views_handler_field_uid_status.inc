<?php
/**
 * @file
 * Views field handler for User ID status information.
 */

class views_handler_field_uid_status extends views_handler_field {

  /**
   * Overrides views_handler::option_definition().
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['cancelled'] = array(
      'default' => 'Cancelled',
      'translatable' => TRUE,
    );
    $options['anonymous'] = array(
      'default' => 'Anonymous',
      'translatable' => TRUE,
    );
    $options['active'] = array(
      'default' => 'Active',
      'translatable' => TRUE,
    );
    $options['blocked'] = array(
      'default' => 'Blocked',
      'translatable' => TRUE,
    );
    return $options;
  }

  /**
   * Overrides views_handler::options_form().
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['cancelled'] = array(
      '#title' => t('Cancelled accounts'),
      '#description' => t('Enter the text to use for cancelled accounts.'),
      '#type' => 'textfield',
      '#default_value' => $this->options['cancelled']
    );
    $form['anonymous'] = array(
      '#title' => t('Anonymous accounts'),
      '#description' => t('Enter the text to use for anonymous accounts.'),
      '#type' => 'textfield',
      '#default_value' => $this->options['anonymous']
    );
    $form['active'] = array(
      '#title' => t('Active accounts'),
      '#description' => t('Enter the text to use for active accounts.'),
      '#type' => 'textfield',
      '#default_value' => $this->options['active']
    );
    $form['blocked'] = array(
      '#title' => t('Blocked accounts'),
      '#description' => t('Enter the text to use for blocked accounts.'),
      '#type' => 'textfield',
      '#default_value' => $this->options['blocked']
    );
  }

  /**
   * Overrides views_handler_field::query().
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

    $this->field_alias = $this->uid_status_status_alias; // used for click sort
  }

  /**
   * Overrides views_handler_field::render().
   */
  function render($values) {
    $uid = $values->{$this->uid_status_uid_alias};
    if ($uid === NULL) {
      return check_plain($this->options['cancelled']);
    }
    elseif ($uid == 0) {
      return check_plain($this->options['anonymous']);
    }
    else {
      $status = $values->{$this->uid_status_status_alias};
      if ($status) {
        return check_plain($this->options['active']);
      }
      else {
        return check_plain($this->options['blocked']);
      }
    }
  }
}
