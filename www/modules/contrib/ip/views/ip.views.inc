<?php
/**
 * @file
 * Provides ip_tracker view fields and filters.
 */

/**
 * Implements hook_views_data().
 */
function ip_views_data() {
  $data = array();

  // Table: IP tracker:
  $data['ip_tracker']['table']['group'] = t('IP Tracker (user)');
  $data['ip_tracker']['table']['base'] = array(
    'field' => 'uid',
    'title' => t('IP Tracker (user)'),
    'help' => t('IP Tracker (user)'),
    'weight' => -10,
  );

  // Uid field
  $data['ip_tracker']['uid'] = array(
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

  // IP field
  $data['ip_tracker']['ip'] = array(
    'title' => t('IP Address'),
    'help' => t('IP address of user.'),
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

  // Visits
  $data['ip_tracker']['visits'] = array(
    'title' => t('Visits'),
    'help' => t('Total visits'),
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

  // First visit
  $data['ip_tracker']['first_visit'] = array(
    'title' => t('First visit'),
    'help' => t('First visit: registration/login.'),
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

  // Last visit
  $data['ip_tracker']['last_visit'] = array(
    'title' => t('Last visit'),
    'help' => t('Last visit: registration/login.'),
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

  // Last visit
  $data['ip_tracker']['user_count'] = array(
    'title' => t('User count'),
    'help' => t('User count.'),
    'field' => array(
      'handler' => 'IpUserCountField',
      'click sortable' => FALSE,
    ),
  );


  // Table: IP Posts:
  $data['ip_posts']['table']['group'] = t('IP Posts');
  $data['ip_posts']['table']['base'] = array(
    'field' => 'type',
    'title' => t('IP Posts (entity)'),
    'help' => t('IP Posts (entity)'),
    'weight' => -10,
  );

  // Type
  $data['ip_posts']['type'] = array(
    'title' => t('Entity type'),
    'help' => t('Entity type'),
    'field' => array(
      'handler' => 'views_handler_field',
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

  // Id field
  $data['ip_posts']['id'] = array(
    'title' => t('Entity id'),
    'help' => t('Entity id'),
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
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  // IP field
  $data['ip_posts']['ip'] = array(
    'title' => t('IP Address'),
    'help' => t('IP address of user.'),
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

  // Node relationship: ip_posts -> node.
  $data['ip_posts']['node'] = array(
    'title' => t('Node'),
    'help' => t('Node'),
    'relationship' => array(
      'base' => 'node',
      'base field' => 'nid',
      'field' => 'id',
      'id' => 'views_handler_relationship',
      'label' => t('Node'),
      'extra' => array(
        0 => array(
          'left_field' => 'type',
          'value' => 'node',
        ),
      ),
    ),
  );

  // Node relationship: node -> ip_posts.
  $data['node']['ip_posts'] = array(
    'title' => t('IP Posts'),
    'help' => t('IP Posts (node)'),
    'relationship' => array(
      'base' => 'ip_posts',
      'base field' => 'id',
      'field' => 'nid',
      'id' => 'views_handler_relationship',
      'label' => t('IP Posts'),
      'extra' => array(
        0 => array(
          'field' => 'type',
          'value' => 'node',
        ),
      ),
    ),
  );

  // Comment relationship: ip_posts -> comment.
  $data['ip_posts']['comment'] = array(
    'title' => t('Comment'),
    'help' => t('Comment'),
    'relationship' => array(
      'base' => 'comment',
      'base field' => 'cid',
      'field' => 'id',
      'id' => 'views_handler_relationship',
      'label' => t('Comment'),
      'extra' => array(
        0 => array(
          'left_field' => 'type',
          'value' => 'comment',
        ),
      ),
    ),
  );

  // Comment relationship: comment -> ip_posts.
  $data['comment']['ip_posts'] = array(
    'title' => t('IP Posts'),
    'help' => t('IP Posts (comment)'),
    'relationship' => array(
      'base' => 'ip_posts',
      'base field' => 'id',
      'field' => 'cid',
      'id' => 'views_handler_relationship',
      'label' => t('IP Posts'),
      'extra' => array(
        0 => array(
          'field' => 'type',
          'value' => 'comment',
        ),
      ),
    ),
  );

  return $data;
}
