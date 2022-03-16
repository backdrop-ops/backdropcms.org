<?php

/**
 * @file
 * Contains the ip_views_handler_argument_ip2long class.
 */

/**
 * A handler to provide proper displays IP Long values
 */
class Ip2LongArgument extends views_handler_argument {
  function query($group_by = FALSE) {
    $this->ensure_my_table();
    $this->query->add_where($this->options['group'], "$this->table_alias.$this->real_field", ip2long($this->argument));
  }
}
