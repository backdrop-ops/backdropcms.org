<?php

/**
 * @file
 * Drush command for packaging files for release nodes.
 *
 * @author Derek Wright (http://drupal.org/user/46549)
 */

function project_release_drush_command() {
  return array(
    'release-package-run' => array(
      'description' => 'Run queued release packaging. Creates packages AND writes release history XML files.',
    ),
    'release-create-history' => array(
      'description' => 'Creates XML release history for projects.',
      'arguments' => array(
        'project_id' => 'Project, either a Node ID or machine name. Omit to build all projects.',
      ),
    ),
    'release-check-status' => array(
      'description' => 'Check the status and visibility for releases.',
      'arguments' => array(
        'project_id' => 'Project, either a Node ID or machine name. Omit to check all projects.',
      ),
    ),
  );
}

function project_release_drush_help($section) {
  switch ($section) {
    case 'meta:project_release:title':
      return dt('Project release commands');
  }
}


/**
 * Check status (recommended, latest, update status, etc) for releases.
 */
function drush_project_release_release_check_status($project_id = NULL) {
  if (is_null($project_id)) {
    drush_log(dt('Checking release status for all projects.'), 'ok');
    project_release_release_check_status_all();
  }
  else {
    $project_node = project_load($project_id);
    if ($project_node === FALSE) {
      return drush_set_error('INVALID_PROJECT', dt('Project ID @id not found.', array('@id' => $project_id)));
    }
    else {
      drush_log(dt('Checking release status for @title [@machine_name]', array('@title' => $project_node->title, '@machine_name' => $project_node->project['name'])), 'ok');
    }
    project_release_release_check_status_project($project_node);
  }
}

/**
 * Check release status for all branches of all projects.
 *
 * Queries {project_release_supported_versions} for all branches across all
 * projects, groups the branches per-project, and invokes a helper function,
 * project_release_compute_update_status_multiple(), to actually compute the
 * update status of every release on each branch, update the release download
 * table (once per project), etc.
 *
 * @see project_release_compute_update_status_multiple()
 */
function project_release_release_check_status_all() {
  $results = db_query("SELECT nid, tid, major FROM {project_release_supported_versions} WHERE supported = 1");
  $branch_count = 0;
  $project_count = 0;
  $branches = array();
  $current_nid = 0;
  foreach ($results as $branch) {
    $branch_count++;
    if ($current_nid != $branch->nid) {
      if (!empty($branches)) {
        project_release_compute_update_status_multiple($current_nid, $branches);
        $project_count++;
        $branches = array();
      }
      $current_nid = $branch->nid;
    }
    $branches[] = array('api_tid' => $branch->tid, 'major' => $branch->major);
  }
  project_release_compute_update_status_multiple($current_nid, $branches);
  $project_count++;
  drush_log(dt('Checked release status for @branch_count branches across @project_count projects.', array('@branch_count' => $branch_count, '@project_count' => $project_count)), 'ok');
}

/**
 * Check the release status on a single project.
 *
 * Queries {project_release_supported_versions} for all branches for a single
 * project and invokes project_release_compute_update_status_multiple() once
 * to compute the release update status, rebuild the download table, etc.
 *
 * @see project_release_compute_update_status_multiple()
 */
function project_release_release_check_status_project($project) {
  $results = db_query("SELECT tid, major FROM {project_release_supported_versions} WHERE supported = 1 AND nid = :nid", array(':nid' => $project->nid));
  $branches = array();
  foreach ($results as $branch) {
    $branches[] = array('api_tid' => $branch->tid, 'major' => $branch->major);
  }
  project_release_compute_update_status_multiple($project->nid, $branches);
  drush_log(dt('Checked release status for @count branches.', array('@count' => count($branches))), 'ok');
}
