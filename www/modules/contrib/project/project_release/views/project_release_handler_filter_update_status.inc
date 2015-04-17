<?php
/**
 * Filter to provide a list of API versions.
 */
class project_release_handler_filter_update_status extends views_handler_filter_in_operator {
  var $value_form_type = 'select';

  function get_value_options() {
    $this->value_options = array(
      'current' => t('Current'),
      'not-current' => t('Not current'),
      'not-secure' => t('Insecure'),
    );
    return $this->value_options;
  }
}
