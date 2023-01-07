<?php
/**
 * @file
 * Token integration for Project module.
 */

/**
 * Implements hook_token_info().
 */
function project_token_info() {
  // Core tokens for nodes.
  $tokens['project_name'] = array(
    'name' => t('Project name'),
    'description' => t('The machine-name of a project.'),
  );
  $tokens['project_path_name'] = array(
    'name' => t('Project path name'),
    'description' => t('The machine-name a project if it is an official project or the node ID of the project if it is a sandbox.'),
  );
  return array(
    'tokens' => array('node' => $tokens),
  );
}

/**
 * Implements hook_tokens().
 */
function project_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $sanitize = !empty($options['sanitize']);

  $replacements = array();

  if ($type == 'node' && !empty($data['node']->project)) {
    $node = $data['node'];
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'project_name':
          $replacements[$original] = $sanitize ? check_plain($node->project['name']) : $node->project['name'];
          break;

        case 'project_path_name':
          if ($node->project['sandbox']) {
            $replacements[$original] = $node->nid;
          }
          else {
            $replacements[$original] = $sanitize ? check_plain($node->project['name']) : $node->project['name'];
          }
          break;
      }
    }
  }

  return $replacements;
}
