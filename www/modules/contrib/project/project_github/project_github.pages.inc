<?php
/**
 * Page callbacks for Project GitHub integration.
 */

/**
 * Page callback for listening to all GitHub events on a contrib project.
 *
 * @param string $callback
 *   A callable function. Which should be passed the GitHub payload.
 * @param array $expected_events
 *   An array of GitHub event names that the given callable function is able
 *   to handle.
 */
function project_github_listener_callback($callback, array $expected_events) {
  $payload = project_github_get_payload();
  $event = $_SERVER['HTTP_X_GITHUB_EVENT'];

  $result = NULL;
  $errors = array();

  // Always return an OK for ping requests, which aren't an actual webhook.
  if ($event === 'ping') {
    $result = TRUE;
  }
  // Check that the callback accepts the given event.
  elseif (in_array($event, $expected_events)) {
    $result = $callback($errors, $payload);
  }

  // Output the results of the callback.
  if (!isset($result)) {
    backdrop_add_http_header('Status', '501 Not Implemented');
    exit(format_string('This URL only supports the following events: "!expected_events". Please remove the "!event" event from the GitHub Webhook configuration.', array('!expected_events' => implode(', ', $expected_events), '!event' => $event)));
  }
  elseif ($result === FALSE || !empty($errors)) {
    backdrop_add_http_header('Status', '500 Failed');
    print "Unable to process the payload.\n";
    $full_project_name = isset($payload['repository']['name']) ? $payload['repository']['name'] : NULL;
    if ($full_project_name) {
      $github_hook_url = 'https://github.com/' . $full_project_name . '/settings/hooks';
      $github_delivery = $_SERVER['HTTP_X_GITHUB_DELIVERY'];
      print "Check the recent GitHub hook requests at $github_hook_url\n";
      print "X-GitHub-Delivery: $github_delivery\n";
    }
    print "\n";
    print implode("\n\n", $errors);
    exit();
  }

  // In the event of no errors and successful response.
  exit('OK');
}

/**
 * Access callback for all GitHub event webhooks.
 */
function project_github_listener_access() {
  $secret_key = settings_get('project_github_secret_key');
  $access_token = settings_get('project_github_access_token');

  if (!strlen($access_token)) {
    backdrop_add_http_header('Status', '403 Access token not configured');
    exit('A GitHub access token has not be configured for this site. Set one up at https://github.com/settings/applications.');
  }

  if (strlen($secret_key)) {
    // If no signature is provided, access is denied.
    if (empty($_SERVER['HTTP_X_HUB_SIGNATURE'])) {
      backdrop_add_http_header('Status', '403 Signature required');
      exit('Signature required for this request. Check the secret key for your webhook.');
    }

    // Validate the provided signature against the secret key. See the line
    // where this is encrypted at:
    // https://github.com/github/github-services/blob/f3bb3dd780feb6318c42b2db064ed6d481b70a1f/lib/service/http_helper.rb#L77
    $input = file_get_contents('php://input');
    list($algorithm, $expected_hash) = explode('=', $_SERVER['HTTP_X_HUB_SIGNATURE'], 2);
    $actual_hash = hash_hmac($algorithm, $input, $secret_key);
    if ($expected_hash !== $actual_hash) {
      backdrop_add_http_header('Status', '403 Invalid signature');
      exit('The provided secret key did not match the server key.');
    }
  }

  if (empty($_SERVER['HTTP_X_GITHUB_EVENT'])) {
    backdrop_add_http_header('Status', '403 Invalid Event');
    exit('No event header provided.');
  }

  return TRUE;
}

/**
 * Get the full payload from the GitHub POST request.
 */
function project_github_get_payload() {
  $input = file_get_contents('php://input');
  // Handle both Form URL-Encoded and JSON input.
  if (strpos($input, 'payload') === 0) {
    $parsed_input = array();
    parse_str($input, $parsed_input);
    $input = $parsed_input['payload'];
  }
  $payload = @json_decode($input, TRUE);
  if (empty($payload)) {
    backdrop_add_http_header('Status', '500 Corrupt payload');
    exit('Corrupt or empty payload.');
  }
  return $payload;
}

/**
 * Project GitHub web hook callback for release events.
 *
 * @param array $errors
 *   An empty array that should be populated with any error information that
 *   occurs during the request.
 * @param array $payload
 *   The GitHub webhook payload.
 * @return bool
 *   TRUE if no error occurred processing the release, FALSE otherwise.
 */
function _project_github_handle_release(array &$errors, array $payload) {
  $project_node_result = FALSE;
  $release_node_result = FALSE;
  // Create the package zip file. Note this modifies the $payload to include the
  // new package information.
  $package_result = _project_github_create_package($errors, $payload);

  // If packaging was successful, create the project node (done only if needed).
  if ($package_result) {
    $project_node_result = _project_github_create_project($errors, $payload);
  }

  // If a project node was created or already exists, create the release with
  // package created above.
  if ($project_node_result) {
    $release_node_result = _project_github_create_project_release($errors, $payload);
  }

  // For better reporting for project authors who may not have access to the
  // group-wide web hook settings, attempt to upload a report of the failures
  // to the release as an asset.
  if (!empty($errors)) {
    _project_github_upload_error_log($errors, $payload);
  }

  return $package_result && $project_node_result && $release_node_result;
}

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
 * @return bool
 *   TRUE if the package is modified and uploaded successfully. FALSE otherwise.
 */
function _project_github_create_package(array &$errors, array &$payload) {
  $access_token = settings_get('project_github_access_token');

  // The name of the project, e.g. "webform".
  $project_name = $payload['repository']['name'];

  // The release tag name. We accept both with and without a "v" prefix and
  // major version number. e.g. "1.x-2.0.5", "v2.0.5", or "v1.x-2.0.5".
  $tag_name = $payload['release']['tag_name'];

  // The URL to the source zip archive that will be parsed.
  $source_zip_url = $payload['release']['zipball_url'];

  // The URL that lists all the current assets on the GitHub release.
  $assets_url = $payload['release']['assets_url'];

  // The URL to which the archive will be uploaded when processing is finished.
  $upload_url = $payload['release']['upload_url'];

  // Remove any leading "v" from the tag.
  $tag_name = ltrim($tag_name, 'v');

  // Create a temporary directory in which to do our work.
  $tempdir = backdrop_realpath('temporary://');
  $directory = tempnam($tempdir, 'project_pkg_');
  unlink($directory);
  mkdir($directory);

  // Copy the file down locally.
  $result = backdrop_http_request($source_zip_url, array(
    'headers' => array(
      'Authorization' => 'token ' . $access_token,
    ),
  ));
  if ($result->code == 200) {
    file_put_contents($directory . '/original.zip', $result->data);
  }
  $archive = new ZipArchive();
  $archive->open($directory . '/original.zip');
  $archive->extractTo($directory);
  $archive->close();
  unset($archive);

  // Remove the archive now that it's expanded.
  unlink($directory . '/original.zip');

  // Make a list of all files sans the archive.
  $files = file_scan_directory($directory, '/.*/');

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

  // Remove the GitHub placeholder name and specify our file name in the URL.
  $upload_url = preg_replace('/{.*}/', '', $upload_url);
  $upload_url .= '?name=' . $archive_name;

  // Upload the new file.
  $upload_result = backdrop_http_request($upload_url, array(
    'headers' => array(
      'Authorization' => 'token ' . $access_token,
      'Content-Type' => 'application/zip',
    ),
    'method' => 'POST',
    'data' => file_get_contents($archive_path),
  ));

  // Delete the working directory (including the new archive).
  file_unmanaged_delete_recursive($directory);

  $success = FALSE;
  $asset_url = NULL;

  // Successful upload.
  if ($upload_result->code == 201) {
    $success = TRUE;
    $asset_url = $result->data['url'];
  }
  // If a file by the same name already exists, assume it is acceptable.
  elseif ($upload_result->code == 400 && $upload_result->error === 'status code 422') {
    $success = TRUE;
  }
  // GitHub can sometimes return a 502 even when the upload was successful.
  // See https://github.com/jutzig/github-release-plugin/issues/5.
  // Check if the file was uploaded regardless of the error.
  $search_result = backdrop_http_request($assets_url, array(
    'headers' => array(
      'Authorization' => 'token ' . $access_token,
    ),
  ));
  if ($search_result->code == 200) {
    $assets_data = json_decode($search_result->data, TRUE);
    $payload['release']['assets'] = $assets_data;
    foreach ($assets_data as $asset) {
      if ($asset['name'] === $archive_name) {
        $success = TRUE;
        $asset_url = $asset['url'];
        break;
      }
    }
  }

  // Rename the label of the package for clarity.
  // https://developer.github.com/v3/repos/releases/#edit-a-release-asset
  if ($success && $asset_url) {
    // Remove "1.x-" from the tag.
    $short_tag_name = preg_replace('/^(v?\d+\.x\-)/', '', $tag_name);
    $query = array(
      'name' => $archive_name,
      'label' => "Download $project_name $short_tag_name (zip)",
    );
    $rename_result = backdrop_http_request($asset_url, array(
      'method' => 'PATCH',
      'data' => json_encode($query),
      'headers' => array(
        'Authorization' => 'token ' . $access_token,
      ),
    ));
    $success = $rename_result->code == 200;
  }

  if ($success) {
    watchdog('project_github', 'Successfully created release for @project @tag and pushed the package to GitHub.', array('@project' => $project_name, '@tag' => $tag_name), WATCHDOG_INFO);
  }
  else {
    watchdog('project_github', 'Error creating release for @project @tag.', array('@project' => $project_name, '@tag' => $tag_name), WATCHDOG_INFO);
    $errors[] = 'Upload HTTP request: ' . print_r($upload_result, 1);
    if (isset($search_result)) {
      $errors[] = 'Asset list request: ' . print_r($search_result, 1);
    }
    if (isset($rename_result)) {
      $errors[] = 'Rename asset request: ' . print_r($rename_result, 1);
    }
  }
  return $success;
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
  $access_token = settings_get('project_github_access_token');

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
 * Project GitHub web hook callback for push events.
 *
 * @param array $errors
 *   An empty array that should be populated with any error information that
 *   occurs during the request.
 * @param array $payload
 *   The GitHub webhook payload.
 * @return bool
 *   TRUE if the package is modified and uploaded successfully. FALSE otherwise.
 */
function _project_github_handle_push(array &$errors, array $payload) {
  $result = _project_github_update_readme($errors, $payload);
  return $result;
}

/**
 * Given a push event, update the project README.
 *
 * @param array $errors
 *   An empty array that should be populated with any error information that
 *   occurs during the request.
 * @param array $payload
 *   The GitHub webhook payload.
 * @return bool
 *   TRUE if the package is modified and uploaded successfully. FALSE otherwise.
 */
function _project_github_update_readme(array &$errors, array $payload) {
  $success = TRUE;

  // Check if we even have a project that matches for making a release.
  $project_node = project_github_load_project($payload['repository']['full_name']);
  if (!$project_node || empty($project_node->project['github_sync_readme'])) {
    return $success;
  }

  if (project_github_update_readme($project_node)) {
    $project_node->save();
    $success = TRUE;
  }
  else {
    $errors['readme'] = t('Updating the node body field with the README content failed.');
    $success = FALSE;
  }

  return $success;
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
  $access_token = settings_get('project_github_access_token');

  // GitHub doesn't provide a URL for the README, though it is documented under
  // the "contents" API. See https://developer.github.com/v3/repos/contents/
  $readme_url = 'https://api.github.com/repos/' . $node->project['github_path'] . '/readme';

  $options = array(
    'headers' => array(
      'Authorization' => 'token ' . $access_token,
      'Accept' => 'application/vnd.github.v3.html',
    ),
  );
  $readme_result = backdrop_http_request($readme_url, $options);
  if ((int) $readme_result->code === 200) {
    $readme = $readme_result->data;
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
