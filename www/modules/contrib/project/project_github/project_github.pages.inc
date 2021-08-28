<?php
/**
 * Page callbacks for Project GitHub integration.
 */

/**
 * Given a release event, download the zip, add package info, and reupload.
 *
 * @param array $errors
 *   An empty array that should be populated with any error information that
 *   occurs during the request.
 * @param array $payload
 *   The GitHub webhook payload. Note that the "assets" property of this payload
 *   will be modified to include the newly uploaded package, so it may be
 *   referenced by later events without retrieving the asset again.
 * @return array|FALSE
 *   The package information array if successful. FALSE if the upload failed.
 */
function _project_github_create_package(array &$errors, array &$payload) {
  // The name of the project, e.g. "webform".
  $project_name = $payload['repository']['name'];

  // The release tag name. We accept both with and without a "v" prefix and
  // major version number. e.g. "1.x-2.0.5", "v2.0.5", or "v1.x-2.0.5".
  $tag_name = $payload['release']['tag_name'];

  // Remove any leading "v" from the tag.
  $tag_name = ltrim($tag_name, 'v');

  // Create a temporary directory in which to do our work.
  $tempdir = backdrop_realpath('temporary://');
  $directory = tempnam($tempdir, 'project_pkg_');
  unlink($directory);
  mkdir($directory);

  $zip_data = project_github_get_release_zip($payload['repository']['full_name'], $tag_name);
  if ($zip_data === FALSE) {
    $error = format_string('Failed to download ZIP archive for @project @tag_name.', array('@project' => $project_name, '@tag_name' => $tag_name));
    watchdog('project_github', $error, array(), WATCHDOG_INFO);
    return $errors;
  }

  file_put_contents($directory . '/original.zip', $zip_data);
  $archive = new ZipArchive();
  $archive->open($directory . '/original.zip');
  $archive->extractTo($directory);
  $archive->close();
  unset($archive);

  // Remove the archive now that it's expanded.
  unlink($directory . '/original.zip');

  // Make a list of all files sans the archive.
  $files = file_scan_directory($directory, '/.*/', array('nomask' => '/^$/'));

  // Allow other modules to modify the files.
  backdrop_alter('project_github_create_package', $files, $project_name);

  // Individually rezip each file, modifying .info files as needed.
  $archive = new ZipArchive();
  $archive_name = $project_name . '.zip';
  $archive_path = $directory . '/' . $archive_name;
  $archive->open($archive_path, ZipArchive::CREATE);
  foreach ($files as $file) {
    // If the file is a .info file, modify it to add required information.
    $extension = substr($file->filename, strrpos($file->filename, '.') + 1);
    if ($extension === 'info') {
      $info_fp = fopen($file->uri, 'a');
      $additions = array(
        '',
        '; ' . t('Added by !site packaging script on !date', array('!site' => config_get('system.core', 'site_name'), '!date' => date('Y-m-d'))),
        'project = ' . $project_name,
        'version = ' . $tag_name,
        'timestamp = ' . REQUEST_TIME,
        '',
      );
      fwrite($info_fp, implode("\n", $additions));
      fclose($info_fp);
    }

    // Remove the temporary directory name.
    $relative_path = str_replace($directory . '/', '', $file->uri);
    // Remove the GitHub-provided directory name, usually a mix of the project
    // name and the version.
    $relative_path = substr($relative_path, strpos($relative_path, '/') + 1);
    // Add a directory with just the project name, so it will expand with the
    // same name consistently.
    $relative_path = $project_name . '/' . $relative_path;
    $archive->addFile($file->uri, $relative_path);
  }
  $archive->close();

  // Remove "1.x-" from the tag.
  $short_tag_name = preg_replace('/^(v?\d+\.x\-)/', '', $tag_name);
  $archive_title = "Download $project_name $short_tag_name (zip)";

  $new_asset_data = project_github_add_release_zip($payload['repository']['full_name'], $tag_name, $archive_name, $archive_title, file_get_contents($archive_path));

  // Delete the working directory (including the new archive).
  file_unmanaged_delete_recursive($directory);

  if ($new_asset_data) {
    // Modify the payload array to include the new asset.
    $payload['release']['assets'][] = $new_asset_data;
    watchdog('project_github', 'Successfully created release for @project @tag and pushed the package to GitHub.', array('@project' => $project_name, '@tag' => $tag_name), WATCHDOG_INFO);
  }
  else {
    watchdog('project_github', 'Error creating release for @project @tag.', array('@project' => $project_name, '@tag' => $tag_name), WATCHDOG_INFO);
  }
  return $new_asset_data;
}

/**
 * Given a release event, automatically create a project node.
 *
 * @param array $errors
 *   An empty array that should be populated with any error information that
 *   occurs during the request.
 * @param array $payload
 *   The GitHub webhook payload.
 * @return string|FALSE
 *   The one of the constants: GITHUB_PROJECT_CREATED, GITHUB_PROJECT_EXISTS, or
 *   GITHUB_PROJECT_FAILED (FALSE).
 */
function _project_github_create_project(array &$errors, array $payload) {
  // Check if we even have a project that matches for making a release.
  $project_node = project_github_load_project($payload['repository']['full_name']);
  if ($project_node) {
    return GITHUB_PROJECT_EXISTS;
  }

  // @todo: Create a different type based on available project types?
  // How would we identify which type of project should be created? For now,
  // we rely on custom modules to use hook_node_presave() to modify the node
  // type.
  $types = project_project_node_types();
  $type = reset($types);

  $project_node = new Node(array(
    'type' => $type,
    'title' => $payload['repository']['name'],
    'uid' => 1,
    'status' => 1,
    'project' => array(
      'name' => $payload['repository']['name'],
      'sandbox' => 0,
      'releases_enabled' => 1,
      'github_path' => $payload['repository']['full_name'],
      'github_sync_readme' => 1,
    ),
  ));
  project_github_update_readme($project_node);

  // Manually validate.
  project_validate_project_title($project_node, $errors);
  project_validate_project_shortname($project_node, $errors);

  // Allow other modules to set errors or modify the node before save.
  foreach (module_implements('github_project_validate') as $module) {
    $function = $module . '_github_project_validate';
    $function($project_node, $errors, $payload);
  }

  if (empty($errors) && $project_node->save()) {
    $success = GITHUB_PROJECT_CREATED;
  }
  else {
    $success = GITHUB_PROJECT_FAILED;
  }

  return $success;
}

/**
 * Given a release event, automatically create a release node for the project.
 *
 * @param array $errors
 *   An empty array that should be populated with any error information that
 *   occurs during the request.
 * @param array $payload
 *   The GitHub webhook payload.
 * @return bool
 *   TRUE if the release node is created. FALSE otherwise.
 */
function _project_github_create_project_release(array &$errors, array $payload) {
  $success = TRUE;

  // Check if we even have a project that matches for making a release.
  $project_node = project_github_load_project($payload['repository']['full_name']);
  if (!$project_node) {
    return $success;
  }

  // Determine the type of release node to be created.
  $project_node_type = node_type_get_type($project_node);
  $release_type_name = $project_node_type->settings['project_release_type'];

  // If releases are not enabled return success.
  if (!$release_type_name) {
    return $success;
  }

  // If the release type is not found, return an error.
  $release_node_type = node_type_get_type($release_type_name);
  if (!$release_node_type) {
    $success = FALSE;
    $errors[] = format_string('The "!type" node type does not exist, so a release could not be created.', array('!type' => $release_node_type));
    return $success;
  }

  $tag_name = $payload['release']['tag_name'];

  // Determine the release zip file, whether it's the first "asset" or the
  // zipball_url otherwise.
  $zip_url = '';
  $zip_size = NULL;
  foreach ($payload['release']['assets'] as $asset) {
    if (strpos($asset['content_type'], 'zip') !== FALSE) {
      $zip_url = $asset['browser_download_url'];
      $zip_size = $asset['size'];
    }
  }
  if (empty($zip_url)) {
    $zip_url = $payload['release']['zipball_url'];
    $zip_size = NULL; // Not provided by GitHub currently.
  }

  // Check that we're not about to overwrite an existing node. We will update
  // the existing node with the same version string if applicable.
  $existing_nid = db_query("SELECT nid FROM {project_release} WHERE version = :version AND project_nid = :project_nid", array(':version' => $payload['release']['tag_name'], ':project_nid' => $project_node->nid))->fetchField();
  if ($existing_nid) {
    $node = node_load($existing_nid);
  }
  else {
    $node = new Node(array(
      'title' => $project_node->project['name'] . ' ' . $payload['release']['tag_name'],
      'type' => $release_type_name,
      'uid' => $project_node->uid,
      'project_release' => array(),
      'status' => 1,
    ));
  }

  $project_release_data = array(
    'project_nid' => $project_node->nid,
    'version' => $tag_name,
    'release_link' => $payload['release']['html_url'],
    'download_link' => $zip_url,
    'download_size' => $zip_size,
    // @todo: Support copying the release notes locally.
    // This has issues because the release notes are in Markdown, not rendered
    // HTML, and we'd need to deal with updates, but GitHub doesn't send us
    // information on release updates, only on creation.
    //'body' => array(LANGUAGE_NONE => array(
    //  'value' => $payload['release']['body'],
    //)),
  );

  $node->project_release = array_merge($node->project_release, $project_release_data);

  // Validate the version string before saving. Because we don't know where in
  // the string the API version is, we have to validate against each API version
  // until we find one that matches (if any).
  $api_versions = project_release_get_api_versions($node->type);
  $version_parts = FALSE;
  if ($api_versions) {
    foreach ($api_versions as $version_api) {
      $node->project_release['version_api'] = $version_api;
      if ($version_parts = project_release_parse_version($node)) {
        break;
      }
    }
  }
  else {
    $version_parts = project_release_parse_version($node);
  }

  if (!$version_parts) {
    $success = FALSE;
    $errors[] = format_string('The version string "!tag" does not match the required format of the "!type" node type.', array('!tag' => $tag_name, '!type' => $node->type));
  }

  // Allow other modules to set errors or modify the node before save.
  foreach (module_implements('github_project_release_validate') as $module) {
    $function = $module . '_github_project_release_validate';
    $function($node, $errors, $payload);
  }

  if (empty($errors)) {
    $success = (bool) $node->save();
    // If the release is saved successfully, update the project node to trigger
    // a rebuild of the current version information.
    if ($success) {
      $project_node->updated = REQUEST_TIME;
      $project_node->save();
    }
  }

  if ($success) {
    watchdog('project_github', 'Created @node_type for @project @tag.', array('@node_type' => $release_type_name, '@project' => $project_node->title, '@tag' => $tag_name), WATCHDOG_INFO);
  }
  else {
    watchdog('project_github', 'Error creating @node_type @project @tag. <pre>@node</pre>', array('@node_type' => $release_type_name, '@project' => $project_node->title, '@tag' => $payload['release']['tag_name'], '@node' => print_r($node, 1)), WATCHDOG_INFO);
    $errors[] = 'Unable to save release node: ' . print_r($node, 1);
  }
  return $success;
}

/**
 * Attach an error log to a GitHub release.
 */
function _project_github_upload_error_log(array $errors, array $payload) {
  // Get access token from githubapi.
  $access_token = githubapi_get_token();

  $error_log = '';
  $error_log .= format_string("Errors occurred while packaging this project on !sitename.\n", array('!sitename' => config_get('system.core', 'site_name')));
  $full_project_name = isset($payload['repository']['name']) ? $payload['repository']['name'] : NULL;
  if ($full_project_name) {
    $github_hook_url = 'https://github.com/' . $full_project_name . '/settings/hooks';
    $github_delivery = $_SERVER['HTTP_X_GITHUB_DELIVERY'];
    $error_log .= "Check the recent GitHub hook requests at $github_hook_url for more information.\n";
    $error_log .= "X-GitHub-Delivery: $github_delivery\n";
  }
  $error_log .= "\nYou may delete this error log by editing this release on GitHub. A separate log is kept in the hook request history.\n\n";
  $error_log .= "\nAfter resolving the below errors, create another release to try packaging again.\n\n";
  $error_log .= "\nReported errors:\n";
  $error_log .= implode("\n\n", $errors);
  // Remove the GitHub placeholder name and specify our file name in the URL.
  $upload_url = preg_replace('/{.*}/', '?name=PACKAGING_ERROR_LOG.txt', $payload['release']['upload_url']);

  // Upload the new file.
  $upload_result = backdrop_http_request($upload_url, array(
    'headers' => array(
      'Authorization' => 'token ' . $access_token,
      'Content-Type' => 'text/plain',
    ),
    'method' => 'POST',
    'data' => $error_log,
  ));

  return $upload_result == 200;
}

/**
 * Update a project node's README from GitHub.
 *
 * @param Node $node
 *   The project node whose README will be updated. Modified by reference.
 * @return bool
 *   TRUE if the node has been modified with the updated README.
 */
function project_github_update_readme(Node &$node) {
  // Convert OWNER/REPO to array.
  $repo = project_github_convert_full_name($node->project['github_path']);
  $githubapi = githubapi_get_class($repo);
  if ($readme = $githubapi->getReadme()) {
    // Remove the wrapping div tag (the only one in the README).
    $readme = preg_replace('/(<div[^>]*?>)/ms', '', $readme);
    $readme = str_replace('</div>', '', $readme);
    // Remove the wrapping article tag.
    $readme = preg_replace('/(<article[^>]*?>)/ms', '', $readme);
    $readme = str_replace('</article>', '', $readme);
    // Remove empty GitHub placeholder anchor tags.
    $readme = preg_replace('/(<span[^>]*?>)(<\/span>)/ms', '', $readme);
    $readme = preg_replace('/(<a[^>]*?>)(<\/a>)/ms', '', $readme);
    // Remove H1 headlines, which should only be the project name.
    $readme = preg_replace('/(<h1[^>]*>)(.*)?(<\/h1>)/ms', '', $readme);

    // Trim newlines.
    $readme = trim($readme);

    // Set the body field with the cleaned-up HTML version of the README.
    $node->body[LANGUAGE_NONE][0]['value'] = $readme;

    // Set the format if one is not yet specified.
    if (!isset($node->body[LANGUAGE_NONE][0]['format'])) {
      $anonymous = backdrop_anonymous_user();
      $node->body[LANGUAGE_NONE][0]['format'] = filter_default_format($anonymous);
    }

    return TRUE;
  }
  return FALSE;
}

/**
 * Convert full_name to array.
 *
 * @param $repository_full_name
 *   full_name property for repository arrived from GitHub API.
 * @return array
 *   - owner: repository owner.
 *   - name: repository name.
 *   FALSE if input string is not OWNER/REPO format
 */
function project_github_convert_full_name($repository_full_name) {
  $path_elements = explode("/", $repository_full_name);
  if (count($path_elements) != 2) {
    return FALSE;
  }

  return array(
    'owner' => $path_elements[0],
    'name' => $path_elements[1],
  );
}

/**
 * Get content of Zip file for release by tag name.
 *
 * @param string $repository_full_name
 *   full_name property for repository arrived from GitHub API.
 * @param string $tag
 *   tag name for requested zip file.
 * @return string|FALSE
 *   The zip file content as a string if the archive was found. FALSE if failed
 *   to download.
 */
function project_github_get_release_zip($repository_full_name, $tag) {
  $repo = project_github_convert_full_name($repository_full_name);
  $githubapi = githubapi_get_class($repo);
  if ($archive_contents = $githubapi->getArchive(array(':ref' => $tag))) {
    return $archive_contents;
  }
  return FALSE;
}

/**
 * Post new asset to release.
 *
 * @param string $repository_full_name
 *   The full_name property for repository arrived from GitHub API.
 * @param string $tag
 *   The tag name for requested zip file.
 * @param string $filename
 *   The file name of the attachment when shown on the GitHub release.
 * @param string $label
 *   The label of the attachment when shown on the GitHub release.
 * @param string $content
 *   The full content of the archive as a string.
 * @return array|FALSE
 *   The newly added asset information response from GitHub. Or FALSE if the
 *   asset upload failed.
 */
function project_github_add_release_zip($repository_full_name, $tag, $filename, $label, $content) {
  $repo = project_github_convert_full_name($repository_full_name);
  $githubapi = githubapi_get_class($repo);

  $add_asset = array(
    ":tag" => $tag,
    'label' => $label,
    'filename' => $filename,
    'content' => $content,
  );

  // GitHub API returns a stdClass on success. Convert to an array to match
  // the format of other $payload data variables.
  $result = $githubapi->addAsset($add_asset);
  if ($result) {
    return (array) $result;
  }
  else {
    return FALSE;
  }
}
