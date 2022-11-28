<?php
/**
 * @file
 * Provides ip_demerits view fields and filters.
 */

/**
 * Implements hook_views_data().
 */
function ip_demerits_views_data() {
  $data = array();

  //
  // Table: Individual demerits
  //
  $data['ip_demerits']['table']['group'] = t('IP demerits');
  $data['ip_demerits']['table']['base'] = array(
    'field' => 'ip',
    'title' => t('IP demerits'),
    'help' => t('Demerits assigned to IP addresses and/or users'),
  );

  // Demerit ID field
  $data['ip_demerits']['demerit_id'] = array(
    'title' => t('Demerit id'),
    'help' => t('Unique id of demerit'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
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

  // Bulk operations on individual demerits
  $data['ip_demerits']['bulk_form'] = array(
    'title' => t('Bulk operations'),
    'help' => t('Provides a checkbox on each row and a form for executing bulk actions on demerits.'),
    'real field' => 'demerit_id',
    'field' => array(
      'handler' => 'views_handler_field_bulk_form',
      'click sortable' => FALSE,
    ),
  );

  // IP field
  $data['ip_demerits']['ip'] = array(
    'title' => t('IP Address'),
    'help' => t('IP address of user.'),
    'relationship' => array(
      'base' => 'ip_tracker',
      'field' => 'ip',
      'handler' => 'views_handler_relationship',
      'label' => t('IP address'),
    ),
    'field' => array(
      'handler' => 'Long2IpField',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'Ip2LongFilter',
    ),
    'argument' => array(
      'handler' => 'Ip2LongArgument',
    ),
  );

  // Uid field
  $data['ip_demerits']['uid'] = array(
    'title' => t('User id'),
    'help' => t('Unique id of user'),
    'relationship' => array(
      'base' => 'users',
      'field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('User uid'),
    ),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
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

  // Uid status field
  $data['ip_demerits']['uid_status'] = array(
    'title' => t('User id-based status'),
    'help' => t('Status of user identified by uid (distinguishing anonymous vs cancelled)'),
    'real field' => 'uid',
    'field' => array(
      'handler' => 'views_handler_field_uid_status',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_uid_status',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_uid_name',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_uid_status',
    ),
  );

  // Uid name field
  $data['ip_demerits']['uid_name'] = array(
    'title' => t('User id-based name'),
    'help' => t('Name of user identified by uid (distinguishing anonymous vs cancelled)'),
    'real field' => 'uid',
    'field' => array(
      'handler' => 'views_handler_field_uid_name',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_uid_name',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_uid_name',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_uid_name',
    ),
  );

  // Points
  $data['ip_demerits']['points'] = array(
    'title' => t('Points'),
    'help' => t('Points for this demerit'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  // Date
  $data['ip_demerits']['date'] = array(
    'title' => t('Date'),
    'help' => t('Date that demerits were assigned'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  // Reason
  $data['ip_demerits']['reason'] = array(
    'title' => t('Reason'),
    'help' => t('Reason for assignment of demerits'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // User relationship: users -> ip_demerits.
  $data['users']['ip_demerits'] = array(
    'title' => t('IP demerits'),
    'help' => t('IP demerits (users)'),
    'relationship' => array(
      'base' => 'ip_demerits',
      'base field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('IP demerits'),
      'title' => t('Referenced demerit'),
      'help' => t('Demerit referenced by this user'),
    ),
  );

  //
  // Table: Total points by IP address
  //
  $data['ip_demerits_ip_totals']['table']['group'] = t('IP demerits IP totals');
  $data['ip_demerits_ip_totals']['table']['base'] = array(
    'field' => 'ip',
    'title' => t('IP demerits IP totals'),
    'help' => t('IP demerits IP totals contains total demerit points by IP'),
  );

  // IP field
  $data['ip_demerits_ip_totals']['ip'] = array(
    'title' => t('IP Address'),
    'help' => t('IP address of user.'),
    'relationship' => array(
      'base' => 'ip_tracker',
      'field' => 'ip',
      'handler' => 'views_handler_relationship',
      'label' => t('IP address'),
    ),
    'field' => array(
      'handler' => 'Long2IpField',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'Ip2LongFilter',
    ),
    'argument' => array(
      'handler' => 'Ip2LongArgument',
    ),
  );

  // Points
  $data['ip_demerits_ip_totals']['points'] = array(
    'title' => t('Points'),
    'help' => t('Points for this demerit'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  // IP relationship: ip_tracker -> ip_demerits_ip_totals
  $data['ip_tracker']['ip_demerits_ip_totals'] = array(
    'title' => t('IP demerits totals'),
    'help' => t('IP demerits totals (IP)'),
    'relationship' => array(
      'base' => 'ip_demerits_ip_totals',
      'base field' => 'ip',
      'handler' => 'views_handler_relationship',
      'label' => t('IP demerit point totals'),
      'title' => t('Referenced demerit points total'),
      'help' => t('Demerit points total referenced by this IP'),
    ),
  );

  //
  // Table: Total points by user ID
  //
  $data['ip_demerits_uid_totals']['table']['group'] = t('IP demerits user totals');
  $data['ip_demerits_uid_totals']['table']['base'] = array(
    'field' => 'uid',
    'title' => t('IP demerits user totals'),
    'help' => t('IP demerits user totals contains total demerit points by user'),
  );

  // Uid field
  $data['ip_demerits_uid_totals']['uid'] = array(
    'title' => t('User id'),
    'help' => t('Unique id of user'),
    'relationship' => array(
      'base' => 'users',
      'field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('User uid'),
    ),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
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

  // Uid status field
  $data['ip_demerits_uid_totals']['uid_status'] = array(
    'title' => t('User id-based status'),
    'help' => t('Status of user identified by uid (distinguishing anonymous vs cancelled)'),
    'real field' => 'uid',
    'field' => array(
      'handler' => 'views_handler_field_uid_status',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_uid_status',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_uid_name',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_uid_status',
    ),
  );

  // Uid name field
  $data['ip_demerits_uid_totals']['uid_name'] = array(
    'title' => t('User id-based name'),
    'help' => t('Name of user identified by uid (distinguishing anonymous vs cancelled)'),
    'real field' => 'uid',
    'field' => array(
      'handler' => 'views_handler_field_uid_name',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_uid_name',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_uid_name',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_uid_name',
    ),
  );

  // Points
  $data['ip_demerits_uid_totals']['points'] = array(
    'title' => t('Points'),
    'help' => t('Points for this demerit'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  // User relationship: users -> ip_demerits_uid_totals
  $data['users']['ip_demerits_ip_totals'] = array(
    'title' => t('IP demerits'),
    'help' => t('IP demerits (users)'),
    'relationship' => array(
      'base' => 'ip_demerits_uid_totals',
      'base field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('IP demerit point totals'),
      'title' => t('Referenced IP points total'),
      'help' => t('Demerit points total referenced by this user'),
    ),
  );

  return $data;
}
