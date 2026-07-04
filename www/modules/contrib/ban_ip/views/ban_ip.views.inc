<?php
/**
 * @file
 * Views data for the Ban IP module.
 */

/**
 * Implements hook_views_data().
 */
function ban_ip_views_data() {

  $data['blocked_ips']['table']['group'] = t('Blocked IP addresses');

  $data['blocked_ips']['table']['base'] = array(
    'field' => 'iid',
    'title' => t('Blocked IP addresses'),
    'help' => t('Individual blocked IP addresses.'),
  );

  $data['blocked_ips']['iid'] = array(
    'title' => t('IP address id'),
    'help' => t('IP address id'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  $data['blocked_ips']['ip'] = array(
    'title' => t('IP address'),
    'help' => t('IP address'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_string',
    ),
  );

  return $data;
}
