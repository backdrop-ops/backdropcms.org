<?php
/**
 * @file
 * Views integration for project module.
 */

/**
 * Implements hook_views_data().
 *
 * Exposes all fields to the views system.
 */
function project_views_data() {
  $data = array();

  $data['project']['table']['group'] = t('Project');

  $data['project']['table']['base'] = array(
    'field' => 'nid', // This is the identifier field for the view.
    'title' => t('Project data'),
    'help' => t('Project table contains data about project nodes.'),
    'weight' => -10,
  );

  $data['project']['table']['join'] = array(
    // Index this array by the table name to which this table refers.
    // 'left_field' is the primary key in the referenced table.
    // 'field' is the foreign key in this table.
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
  );

  // Node ID table field.
  $data['project']['nid'] = array(
    'title' => t('Project NID'),
    'help' => t('Project that references a node.'),
    // Define a relationship to the {node} table, so example_table views can
    // add a relationship to nodes. If you want to define a relationship the
    // other direction, use hook_views_data_alter(), or use the 'implicit' join
    // method described above.
    'relationship' => array(
      'base' => 'node', // The name of the table to join with.
      'base field' => 'nid', // The name of the field on the joined table.
      'handler' => 'views_handler_relationship',
      'label' => t('Project'),
      'title' => t('Project information'),
    ),
  );

  $data['project']['name'] = array(
    'title' => t('Project name'),
    'help' => t('The machine name of a project node.'),
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

  $data['project']['sandbox'] = array(
    'title' => t('Project type'),
    'help' => t('Whether or not the project is a sandbox project or a full project.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
      'output formats' => array(
        'sandbox-notsandbox' => array(t('Sandbox project'), t('Full project')),
      ),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Is sandbox'),
      'type' => 'yes-no',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  return $data;
}

/**
 * Implements hook_views_plugins().
 */
function project_views_plugins() {
  return array(
    'argument validator' => array(
      'project' => array(
        'title' => t('Project'),
        'handler' => 'project_plugin_argument_validate',
      ),
    ),
  );
}
