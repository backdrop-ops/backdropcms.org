<?php
/**
 * @file
 * Views integration for project module.
 */

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
