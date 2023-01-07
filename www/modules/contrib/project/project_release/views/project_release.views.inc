<?php

/**
 * @file
 * Provides support for Views integration.
 */

/**
 * Implements hook_views_data().
 *
 * Exposes all fields to the views system.
 */
function project_release_views_data() {
  $data = array();

  $data['project_release']['table']['group'] = t('Project release');

  $data['project_release']['version'] = array(
    'title' => t('Version'),
    'help' => t('The full version number for the release.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => FALSE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter',
    ),
  );
  $data['project_release']['project_nid'] = array(
    'title' => t('Project node'),
    'help' => t('Relate a project release to its parent project.'),
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_node_nid',
      'name field' => 'title',
      'numeric' => TRUE,
      'validate type' => 'nid',
    ),
    'relationship' => array(
      'base' => 'node',
      'base field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Project node'),
    ),
  );
  $data['project_release']['version_api'] = array(
    'title' => t('API version'),
    'help' => t('The supported API version.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => FALSE,
    ),
    'filter' => array(
      'handler' => 'project_release_handler_filter_version_api',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['project_release']['version_major'] = array(
    'title' => t('Major version number'),
    'help' => t('The "major" portion of a version number.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['project_release']['version_minor'] = array(
    'title' => t('Minor version number'),
    'help' => t('The "minor" portion of a version number.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['project_release']['version_patch'] = array(
    'title' => t('Patch version number'),
    'help' => t('The "patch" portion of a version number. e.g. alpha1, beta2, RC3.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['project_release']['update_status'] = array(
    'title' => t('Update status'),
    'help' => t('Indicator if this release is the latest available version. Either "current", "not-current", or "not-secure".'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'project_release_handler_filter_update_status',
    ),
  );
  $data['project_release']['security_update'] = array(
    'title' => t('Is security update'),
    'help' => t('Indicator if this release contains security fixes.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'type' => 'yes-no',
    ),
  );
  $data['project_release']['release_link'] = array(
    'title' => t('Release link'),
    'help' => t('URL to the release information page.'),
    'field' => array(
      'handler' => 'views_handler_field_url',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter',
    ),
  );
  $data['project_release']['download_link'] = array(
    'title' => t('Download link'),
    'help' => t('URL to the release download file.'),
    'field' => array(
      'handler' => 'views_handler_field_url',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter',
    ),
  );
  $data['project_release']['download_size'] = array(
    'title' => t('Download size'),
    'help' => t('File size of the release download file.'),
    'field' => array(
      'handler' => 'views_handler_field_file_size',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['project']['releases_enabled'] = array(
    'title' => t('Releases enabled'),
    'help' => t('Projects that have releases may have downloadable files attached to them.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Releases enabled'),
      'type' => 'enabled-disabled',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['project_release_supported_versions']['table']['group'] = t('Project release');
  $data['project_release_supported_versions']['nid'] = array(
    'title' => t('Project ID'),
    'help' => t('Project node ID of the supported version information.'),
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_node_nid',
      'name field' => 'title',
      'numeric' => TRUE,
      'validate type' => 'nid',
    ),
    'relationship' => array(
      'base' => 'node',
      'handler' => 'views_handler_relationship',
      'label' => t('Project node'),
    ),
  );
  $data['project_release_supported_versions']['version_api'] = array(
    'title' => t('API version'),
    'help' => t('The supported API version.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => FALSE,
    ),
    'filter' => array(
      'handler' => 'project_release_handler_filter_version_api',
    ),
  );
  $data['project_release_supported_versions']['supported'] = array(
    'title' => t('Supported'),
    'help' => t('Is the specific branch supported by a given project.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'type' => 'yes-no',
    ),
  );
  $data['project_release_supported_versions']['recommended'] = array(
    'title' => t('Recommended branch'),
    'help' => t('Is the specific branch the recommended branch for a given project.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'type' => 'yes-no',
    ),
  );
  $data['project_release_supported_versions']['recommended_release'] = array(
    'title' => t('Recommended release'),
    'help' => t('The recommended release from a specific branch of a project.'),
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_node_nid',
      'name field' => 'title',
      'numeric' => TRUE,
      'validate type' => 'nid',
    ),
    'relationship' => array(
      'base' => 'node',
      'handler' => 'views_handler_relationship',
      'label' => t('Recommended release node'),
    ),
  );
  $data['project_release_supported_versions']['latest_release'] = array(
    'title' => t('Latest release'),
    'help' => t('The latest release from a specific branch of a project.'),
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_node_nid',
      'name field' => 'title',
      'numeric' => TRUE,
      'validate type' => 'nid',
    ),
    'relationship' => array(
      'base' => 'node',
      'handler' => 'views_handler_relationship',
      'label' => t('Latest release node'),
    ),
  );
  $data['project_release_supported_versions']['latest_security_release'] = array(
    'title' => t('Latest security release'),
    'help' => t('The latest security release from a specific branch of a project (if any).'),
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_node_nid',
      'name field' => 'title',
      'numeric' => TRUE,
      'validate type' => 'nid',
    ),
    'relationship' => array(
      'base' => 'node',
      'handler' => 'views_handler_relationship',
      'label' => t('Latest security release node'),
    ),
  );
  $data['project_release_supported_versions']['release_type'] = array(
    'title' => t('Latest or recommended release'),
    'help' => t('Filter based on the latest or recommended release from a specific branch of a project.'),
    'filter' => array(
      'handler' => 'project_release_handler_filter_release_node_nid',
    ),
  );

  return $data;
}

/**
 * Implements hook_views_data_alter().
 */
function project_release_views_data_alter(&$data) {
  $data['node']['project_releases'] = array(
    'title' => t('Release information'),
    'help' => t('Relate a project release to its release information.'),
    'relationship' => array(
      'group' => t('Project release'),
      'label' => t('Project releases'),
      'base' => 'project_release',
      'base field' => 'nid',
      'relationship field' => 'nid',
      'handler' => 'views_handler_relationship',
    ),
  );

  $data['node']['project_release_supported_versions'] = array(
    'title' => t('Supported releases'),
    'help' => t('Relate a parent project node to its currently supported releases.'),
    'relationship' => array(
      'group' => t('Project release'),
      'label' => t('Project releases'),
      'base' => 'project_release_supported_versions',
      'base field' => 'nid',
      'relationship field' => 'nid',
      'handler' => 'views_handler_relationship',
    ),
  );

  // Add a relationship for information and filters about supported releases.
  $data['field_data_field_release_project']['supported_releases_rel'] = array(
    'group' => t('Project release'),
    'title' => t('Supported releases'),
    'help' => t('Create a relationship to more information about supported releases.'),
    'relationship' => array(
      'base' => 'project_release_supported_versions',
      'base field' => 'nid',
      'relationship field' => 'field_release_project_target_id',
      'handler' => 'project_release_handler_relationship_supported_versions',
      'label' => t('supported versions'),
    ),
  );
}
