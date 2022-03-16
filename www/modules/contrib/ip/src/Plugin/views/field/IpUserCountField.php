<?php

/**
 * @file
 * Contains the ip_views_handler_field_user_count class.
 */

/**
 * A handler to provide proper displays IP Long values
 */
class IpUserCountField extends views_handler_field {

  function query() {
    // @TODO: do ip_tracker_ip_user_count() as a subquery!
    $this->field_alias = $this->query->add_field('ip_tracker', 'ip',
      $this->table_alias . '_' . $this->field);
  }

  function render($values) {
    $value = $this->get_value($values);
    $count = !empty($value) ? ip_tracker_ip_user_count(long2ip($value)) : 0;

    return $count;
  }
}
