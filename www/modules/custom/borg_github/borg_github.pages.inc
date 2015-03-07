<?php
/**
 * Page callbacks for BackdropCMS.org GitHub integration.
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
function borg_github_listener_callback($callback, array $expected_events) {
  $payload = borg_github_get_payload();
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
function borg_github_listener_access() {
  $secret_key = settings_get('borg_github_webhook_secret_key', '');
  $access_token = settings_get('borg_github_access_token');

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
    $secret_key = settings_get('borg_github_secret_key', 'foo');
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
function borg_github_get_payload() {
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
 * Given a project zip archive, download it, add package info, and reupload.
 *
 * @param array $errors
 *   An empty array that should be populated with any error information that
 *   occurs during the request.
 * @param array $payload
 *   The GitHub webhook payload.
 * @return bool
 *   TRUE if the package is modified and uploaded successfully. FALSE otherwise.
 */
function _borg_github_create_package(array &$errors, array $payload) {
  $access_token = settings_get('borg_github_access_token');

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
  $directory = tempnam($tempdir, 'borg_pkg_');
  unlink($directory);
  mkdir($directory);

  // Copy the file down locally.
  $result = backdrop_http_request($source_zip_url);
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
        '; Added by BackdropCMS.org packaging script on ' . date('Y-m-d'),
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
  $upload_url = str_replace('{?name}', '', $upload_url);
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
  elseif ($upload_result->code == 502) {
    $search_result = backdrop_http_request($assets_url, array(
      'headers' => array(
        'Authorization' => 'token ' . $access_token,
      ),
    ));
    if ($search_result->code == 200) {
      $release_data = json_decode($search_result->data, TRUE);
      foreach ($release_data as $asset) {
        if ($asset['name'] === $archive_name) {
          $success = TRUE;
          $asset_url = $asset['url'];
          break;
        }
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
    watchdog('github', 'Successfully created release for @project @tag and pushed the package to GitHub.', array('@project' => $project_name, '@tag' => $tag_name), WATCHDOG_INFO);
  }
  else {
    watchdog('github', 'Error creating release for @project @tag.', array('@project' => $project_name, '@tag' => $tag_name), WATCHDOG_INFO);
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
