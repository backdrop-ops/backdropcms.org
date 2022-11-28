<?php
/**
 * @file
 * Views field handler for User ID-based name information.
 */

class views_handler_field_uid_name extends views_handler_field {

  /**
   * Overrides views_handler::option_definition().
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['link'] = array(
      'default' => TRUE,
      'translatable' => FALSE,
    );
    $options['cancelled'] = array(
      'default' => '<cancelled [uid]>',
      'translatable' => TRUE,
    );
    $options['anonymous'] = array(
      'default' => 'Anonymous',
      'translatable' => TRUE,
    );
    return $options;
  }

  /**
   * Overrides views_handler::options_form().
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['link'] = array(
      '#type' => 'checkbox',
      '#title' => t('Link this field to its user'),
      '#description' => t('Enable to link this field to its user if not cancelled or anonymous.'),
      '#default_value' => $this->options['link'],
    );
    $form['cancelled'] = array(
      '#type' => 'textfield',
      '#title' => t('Cancelled text'),
      '#description' => t('Text to display for cancelled users. You can use [uid] as a token for the (former) uid of the user.'),
      '#default_value' => $this->options['cancelled'],
    );
    $form['anonymous'] = array(
      '#type' => 'textfield',
      '#title' => t('Anonymous text'),
      '#description' => t('Text to display for an anonymous user (uid = 0).'),
      '#default_value' => $this->options['anonymous'],
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
    $table_alias = $this->query->add_relationship("{$this->table_alias}__users", $join, 'users');
    $this->uid_name_uid = $this->query->add_field($this->table_alias, 'uid');
    $this->uid_name_uid_alias = $this->query->add_field($table_alias, 'uid');
    $this->uid_name_name_alias = $this->query->add_field($table_alias, 'name');

    $this->field_alias = $this->uid_name_name_alias; // used for click sort
  }

  /**
   * Overrides views_handler_field::render().
   */
  function render($values) {
    $uid = $values->{$this->uid_name_uid};
    $uid_alias = $values->{$this->uid_name_uid_alias};
    if ($uid_alias === NULL) {
      return check_plain(str_replace('[uid]', $uid, $this->options['cancelled']));
    }
    elseif ($uid_alias == 0) {
      return check_plain($this->options['anonymous']);
    }
    else {
      $name = $values->{$this->uid_name_name_alias};
      if ($this->options['link']) {
        return l($name, "user/{$uid}");
      }
      else {
        return check_plain($name);
      }
    }
  }
}
