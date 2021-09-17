<?php

/**
 * @file
 * Contains the ip_views_handler_filter_ip2long class.
 */

/**
 * A handler to provide proper displays IP Long values
 */
class Ip2LongFilter extends views_handler_filter_numeric {
  
  function op_between($field) {
    if ($this->operator == 'between') {
      $this->query->add_where($this->options['group'], $field, array(ip2long($this->value['min']), ip2long($this->value['max'])), 'BETWEEN');
    }
    else {
      $this->query->add_where($this->options['group'], db_or()->condition($field, ip2long($this->value['min']), '<=')->condition($field, ip2long($this->value['max']), '>='));
    }
  }

  function op_simple($field) {
    $this->query->add_where($this->options['group'], $field, ip2long($this->value['value']), $this->operator);
  }

  function op_empty($field) {
    if ($this->operator == 'empty') {
      $operator = "IS NULL";
    }
    else {
      $operator = "IS NOT NULL";
    }

    $this->query->add_where($this->options['group'], $field, NULL, $operator);
  }

  function op_regex($field) {
    $this->query->add_where($this->options['group'], $field, ip2long($this->value['value']), 'RLIKE');
  }
}
