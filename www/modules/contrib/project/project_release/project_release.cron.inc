<?php
/**
 * Callbacks and code used during hook_cron() within Project Release module.
 */

/**
 * Generate the history for a project.
 *
 * @param int|NULL $project_nid
 *   The node ID of the project whose XML history will be generated. If NULL,
 *   then the release history for all projects will be generated.
 *
 * @return mixed
 *
 * @throws Exception
 */
function project_release_release_create_history($project_nid = NULL) {
  // Force the right umask while this script runs, so that everything is created
  // with sane file permissions.
  umask(0022);

  if (is_null($project_nid)) {
    watchdog('project_release', 'Generating XML release history files for all projects.');
  }
  else {
    $project_node = project_load($project_nid);
    if ($project_node === FALSE) {
      watchdog('project_release', 'Project ID @id not found when attempting to generate project XML history.', array('@id' => $project_nid), WATCHDOG_ERROR);
      return FALSE;
    }
  }

  if (isset($project_node)) {
    project_release_history_generate_project_xml($project_node);
  }
  else {
    // Generate all xml files for projects with releases.
    $result = db_query('SELECT nid FROM {project} WHERE sandbox = 0 AND releases_enabled = 1');
    if (!empty($result)) {
      foreach ($result as $project) {
        if ($project_node = node_load($project->nid, NULL, TRUE)) {
          project_release_history_generate_project_xml($project_node);

          // Reset caches so we aren't leaking memory.
          backdrop_static_reset('field_language');
          backdrop_lookup_path('wipe');
        }
      }
    }
  }

  // If we're operating on all projects, generate the huge list, too.
  if (is_null($project_nid)) {
    $all_projects = db_query('SELECT n.title, n.nid, n.status, n.type, p.name AS machine_name, u.name AS username FROM {node} n INNER JOIN {project} p ON n.nid = p.nid INNER JOIN {users} u ON n.uid = u.uid');
    $all_projects_meta = array();
    foreach ($all_projects as $project) {
      $project_meta = array(
        'key' => 'project',
        'value' => array(
          'title' => $project->title,
          'short_name' => $project->machine_name,
          'link' => url('node/' . $project->nid, array('absolute' => TRUE)),
          'dc:creator' => $project->username,
          'type' => $project->type,
          'project_status' => $project->status ? 'published' : 'unpublished',
        ),
      );
      if ($project->status) {
        $api_versions_meta = array();

        // Include a list of supported API versions for this project.
        $project_api_versions = db_query('SELECT DISTINCT version_api FROM project_release WHERE project_nid = :nid', array(':nid' => $project->nid))->fetchCol();
        foreach ($project_api_versions as $api_version) {
          $project_meta['value']['api_versions'][] = array(
            'key' => 'api_version',
            'value' => $api_version,
          );
        }
      }
      backdrop_alter('project_release_project_list_xml', $project_meta, $project, $project_api_versions);
      $all_projects_meta[$project->nid] = $project_meta;
    }
    if (empty($all_projects_meta)) {
      return watchdog('project_release', 'No projects found on this server.');
    }
    $all_projects_meta = array(array(
      'key' => 'projects',
      // Dublin core namespace according to http://dublincore.org/documents/dcmi-namespace/
      'attributes' => array(
        'xmlns:dc' => 'http://purl.org/dc/elements/1.1/',
      ),
      'value' => $all_projects_meta,
    ));

    project_release_history_write_xml($all_projects_meta);
  }

  return TRUE;
}

/**
 * Generate the XML history file for a given project name and API compatibility
 * term.
 *
 * If a history file already exists for this combination, this function will
 * generate a new history and atomically replace the old one (currently, just
 * logs to watchdog for debugging).
 *
 * @param Node $project_node
 *   Project node entity.
 * @param string $version_api
 *   The API compatibility to use, or NULL if versions are considered.
 *
 * @throws Exception
 */
function project_release_history_generate_project_xml(Node $project_node, $version_api = NULL) {
  $releases_xml = array(
    'all' => array(),
  );

  if ($project_node->status) {
    foreach (project_release_query_releases($project_node->nid, $version_api) as $release_node) {
      $release_meta = array(
        'key' => 'release',
        'value' => array(
          'name' => $release_node->title,
          'version' => $release_node->project_release['version'],
          'status' => $release_node->status ? 'published' : 'unpublished',
          'date' => $release_node->created,
          'filesize' => $release_node->project_release['download_size'],
          'security_update' => $release_node->project_release['security_update'] ? 'true' : 'false',
        ),
      );
      foreach (array('major', 'minor', 'patch', 'extra') as $version_type) {
        $version_type = 'version_' . $version_type;
        // 0 is a totally legitimate value for any of these version fields, so
        // we need to test with isset() instead of !empty(). However, we do
        // *not* want to include anything in the release history XML if the
        // value is an empty string.
        if (isset($release_node->project_release[$version_type]) && $release_node->project_release[$version_type] !== '') {
          $release_meta['value'][$version_type] = $release_node->project_release[$version_type];
        }
      }

      if ($release_node->status) {
        // Published, so we should include the links.
        $release_link = !empty($release_node->project_release['release_link']) ? $release_node->project_release['release_link'] : url('node/' . $release_node->nid, array('absolute' => TRUE));
        $release_meta['value']['release_link'] = $release_link;
        $release_meta['value']['download_link'] = $release_node->project_release['download_link'];
      }

      // Backwards-compatibility: Provide security update information as a
      // "term". Update.module cares *only* about security updates, it has no
      // support for the "New features" or "Bug fixes" terms.
      if (!empty($security_update)) {
        $project_meta['terms']['term'] = array(
          'name' => 'Release type',
          0 => array(
            'key' => 'value',
            'value' => 'Security update',
          ),
        );
      }

      // API-specific file.
      $release_version_api = $release_node->project_release['version_api'];
      backdrop_alter('project_release_release_xml', $release_meta, $release_node, $project_node);
      if ($release_version_api) {
        $releases_xml[$release_version_api][] = $release_meta;
      }
      // All releases file.
      $releases_xml['all'][] = $release_meta;
    }
  }

  foreach ($releases_xml as $release_version_api => $release_meta) {
    $project_meta = array(
      'title' => $project_node->title,
      'short_name' => $project_node->project['name'],
      'dc:creator' => $project_node->name,
      'type' => $project_node->type,
      'api_version' => $release_version_api,
      'project_status' => $project_node->status ? 'published' : 'unpublished',
    );

    if ($project_node->status) {
      if ($release_version_api !== 'all') {
        // Include the info about supported and recommended major versions.
        $query = db_query('SELECT version_major, supported, recommended FROM {project_release_supported_versions} WHERE nid = :nid AND version_api = :version_api AND (supported = 1 OR recommended = 1)', array(':nid' => $project_node->nid, ':version_api' => $release_version_api));
        $supported_majors = array();
        $recommended_major = NULL;
        foreach ($query as $version_info) {
          if ($version_info->supported) {
            $supported_majors[] = $version_info->version_major;
          }
          if ($version_info->recommended) {
            $project_meta['recommended_major'] = $version_info->version_major;
          }
        }
        if (empty($supported_majors)) {
          $project_meta['project_status'] = 'unsupported';
        }
        else {
          $project_meta['supported_majors'] = implode(',', $supported_majors);
          // To avoid confusing existing clients, include <default_major>, too.
          $project_meta['default_major'] = min($supported_majors);
        }
      }
      $project_meta['link'] = url('node/' . $project_node->nid, array('absolute' => TRUE));
    }

    if (!empty($release_meta)) {
      $project_meta['releases'] = $release_meta;
    }

    $project = array(0 => array(
      'key' => 'project',
      'attributes' => array('xmlns:dc' => 'http://purl.org/dc/elements/1.1/'),
      'value' => $project_meta,
    ));
    backdrop_alter('project_release_project_xml', $project, $project_node, $release_version_api);
    project_release_history_write_xml($project, $project_node, $release_version_api);
  }
}

/**
 * Write out the XML history for a given project and version to a file.
 *
 * @param array $xml
 *   Array containing the XML representation of the history.
 * @param $project_node
 *   An object containing (at least) the title and project attributes.
 * @param $version_api
 *   The API compatibility version the history is for.
 *
 * @throws Exception
 * @return bool
 */
function project_release_history_write_xml(array $xml, Node $project_node = NULL, $version_api = NULL) {
  $full_xml = '<?xml version="1.0" encoding="utf-8"?>' . "\n";
  $full_xml .= format_xml_elements($xml);
  if (is_null($project_node)) {
    // We are outputting a global project list.
    $project_dir = 'project-list';
    $filename = 'project-list-all.xml';
  }
  else {
    // Setup the filenames we'll be using.
    $project_dir = $project_node->project['name'];
    $filename = $project_node->project['name'] . '-' . strtr($version_api, '/', '_') . '.xml';
  }

  // Make sure we've got the right project-specific subdirectory.
  $project_dir = file_build_uri(settings_get('project_release_history_root', 'release-history') . '/' . $project_dir);
  if (!file_prepare_directory($project_dir, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS)) {
    $wrapper = file_stream_wrapper_get_instance_by_uri($project_dir);
    throw new Exception(t('@path does not exist or is not writable.', array('@path' => $wrapper->realpath())));
  }

  // Write out file.
  if (!file_unmanaged_save_data($full_xml, $project_dir . '/' . $filename, FILE_EXISTS_REPLACE)) {
    $wrapper = file_stream_wrapper_get_instance_by_uri($project_dir . '/' . $filename);
    throw new Exception(t('Can\'t write to @file.', array('@file' => $wrapper->realpath())));
  }

  return TRUE;
}
