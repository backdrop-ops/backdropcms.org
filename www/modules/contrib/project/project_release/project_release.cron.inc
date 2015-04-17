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
    $result = db_query('SELECT n.title, n.nid, n.status, n.type, p.name AS machine_name, u.name AS username FROM {node} n INNER JOIN {project} p ON n.nid = p.nid INNER JOIN {users} u ON n.uid = u.uid');
    $xml = '';
    foreach ($result as $project) {
      $xml .= " <project>\n";
      $xml .= '  <title>' . check_plain($project->title) . "</title>\n";
      $xml .= '  <short_name>' . check_plain($project->machine_name) . "</short_name>\n";
      $xml .= '  <link>' . url('node/' . $project->nid, array('absolute' => TRUE)) . "</link>\n";
      $xml .= '  <dc:creator>' . check_plain($project->username) . "</dc:creator>\n";
      $xml .= '  <type>' . check_plain($project->type) . "</type>\n";
      if (!$project->status) {
        // If it's not published, we can skip the rest for this project.
        $xml .= "  <project_status>unpublished</project_status>\n";
      }
      else {
        $xml .= "  <project_status>published</project_status>\n";
        // Include a list of supported API versions for this project.
        $project_api_versions = db_query('SELECT DISTINCT version_api FROM project_release WHERE project_nid = :nid', array(':nid' => $project->nid))->fetchCol();
        $xml_api_terms = '';
        foreach ($project_api_versions as $api_version) {
          $xml_api_terms .= '   <api_version>' . check_plain($api_version) . "</api_version>\n";
        }
        if (!empty($xml_api_terms)) {
          $xml .= "  <api_versions>\n" . $xml_api_terms . "  </api_versions>\n";
        }
      }

      $xml .= " </project>\n";
    }
    if (empty($xml)) {
      return watchdog('project_release', 'No projects found on this server.');
    }
    project_release_history_write_xml($xml);
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
    'all' => '',
  );

  if ($project_node->status) {
    foreach (project_release_query_releases($project_node->nid, $version_api) as $release_node) {
      $output = "<release>\n";
      $output .= '  <name>' . check_plain($release_node->title) . "</name>\n";
      $output .= '  <version>' . check_plain($release_node->project_release['version']) . "</version>\n";
      foreach (array('major', 'minor', 'patch', 'extra') as $version_type) {
        $version_type = 'version_' . $version_type;
        // 0 is a totally legitimate value for any of these version fields, so
        // we need to test with isset() instead of !empty(). However, we do
        // *not* want to include anything in the release history XML if the
        // value is an empty string.
        if (isset($release_node->project_release[$version_type]) && $release_node->project_release[$version_type] !== '') {
          $output .= "  <$version_type>" . check_plain($release_node->project_release[$version_type]) . "</$version_type>\n";
        }
      }

      if ($release_node->status) {
        // Published, so we should include the links.
        $output .= "  <status>published</status>\n";
        $release_link = !empty($release_node->project_release['release_link']) ? $release_node->project_release['release_link'] : url('node/' . $release_node->nid, array('absolute' => TRUE));
        $output .= '  <release_link>' . check_plain($release_link) . "</release_link>\n";
        $output .= '  <download_link>' . check_plain($release_node->project_release['download_link']) . "</download_link>\n";
      }
      else {
        $output .= "  <status>unpublished</status>\n";
      }
      // We want to include the rest of these regardless of the status.
      $output .= '  <date>' . check_plain($release_node->created) . "</date>\n";
      // We no longer provide an MD5 hash like Drupal 7 did. Update module
      // never used this value though, so restoring it provides little value.
      //$output .= '  <mdhash>'  . check_plain($release->download_md5) . "</mdhash>\n";
      $output .= '  <filesize>' . check_plain($release_node->project_release['download_size']) . "</filesize>\n";

      $security_update = $release_node->project_release['security_update'];
      $output .= "  <security_update>" . ($security_update ? 'true' : 'false') . "</security_update>\n";

      // Backwards-compatibility: Provide security update information as a
      // "term". Update.module cares *only* about security updates, it has no
      // support for the "New features" or "Bug fixes" terms.
      if (!empty($security_update)) {
        $output .= "  <terms>\n";
        $output .= "    <term><name>Release type</name><value>Security update</value></term>\n";
        $output .= "  </terms>\n";
      }

      $output .= "</release>\n";

      // API-specific file.
      $release_version_api = $release_node->project_release['version_api'];
      if ($release_version_api) {
        if (!isset($releases_xml[$release_version_api])) {
          $releases_xml[$release_version_api] = '';
        }
        $releases_xml[$release_version_api] .= $output;
      }
      // All releases file.
      $releases_xml['all'] .= $output;
    }
  }

  foreach ($releases_xml as $release_version_api => $release_xml) {
    $xml = '<title>' . check_plain($project_node->title) . "</title>\n";
    $xml .= '<short_name>' . check_plain($project_node->project['name']) . "</short_name>\n";
    $xml .= '<dc:creator>' . check_plain($project_node->name) . "</dc:creator>\n";
    $xml .= '<type>' . check_plain($project_node->type) . "</type>\n";
    $xml .= '<api_version>' . $release_version_api . "</api_version>\n";

    if ($project_node->status) {
      $project_status = 'published';
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
            $recommended_major = $version_info->version_major;
          }
        }
        if (isset($recommended_major)) {
          $xml .= '<recommended_major>' . $recommended_major . "</recommended_major>\n";
        }
        if (empty($supported_majors)) {
          $project_status = 'unsupported';
        }
        else {
          $xml .= '<supported_majors>' . implode(',', $supported_majors) . "</supported_majors>\n";
          // To avoid confusing existing clients, include <default_major>, too.
          $xml .= '<default_major>' . min($supported_majors) . "</default_major>\n";
        }
      }
      $xml .= '<project_status>' . $project_status . "</project_status>\n";
      $xml .= '<link>' . url('node/' . $project_node->nid, array('absolute' => TRUE)) . "</link>\n";
    }
    else {
      $xml .= "<project_status>unpublished</project_status>\n";
    }

    if (!empty($release_xml)) {
      $xml .= "<releases>\n" . $release_xml . "</releases>\n";
    }

    project_release_history_write_xml($xml, $project_node, $release_version_api);
  }
}

/**
 * Write out the XML history for a given project and version to a file.
 *
 * @param $xml
 *   String containing the XML representation of the history.
 * @param $project_node
 *   An object containing (at least) the title and project attributes.
 * @param $version_api
 *   The API compatibility version the history is for.
 *
 * @throws Exception
 * @return bool
 */
function project_release_history_write_xml($xml, Node $project_node = NULL, $version_api = NULL) {
  // Dublin core namespace according to http://dublincore.org/documents/dcmi-namespace/
  $dc_namespace = 'xmlns:dc="http://purl.org/dc/elements/1.1/"';
  $full_xml = '<?xml version="1.0" encoding="utf-8"?>' . "\n";
  if (is_null($project_node)) {
    // We are outputting a global project list.
    $project_dir = 'project-list';
    $filename = 'project-list-all.xml';
    $full_xml .= '<projects ' . $dc_namespace . ">\n" . $xml . "</projects>\n";
  }
  else {
    // Setup the filenames we'll be using.
    $project_dir = $project_node->project['name'];
    $filename = $project_node->project['name'] . '-' . strtr($version_api, '/', '_') . '.xml';
    $full_xml .= '<project ' . $dc_namespace . ">\n" . $xml . "</project>\n";
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
