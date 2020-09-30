<?php
/**
 * @ file
 * YouTube field helper functions.
 */

/**
 * Extracts the video_id from the submitted field value.
 *
 * @param $input
 *   The input submitted to the field.
 *
 * @return
 *   Returns the video_id if available, or FALSE if not.
 */
function youtube_get_video_id($input) {
  // See README.txt for accepted URL formats.
  preg_match("/^(?:http(?:s)?:\/\/)?(?:www\.)?(?:youtu\.be\/|youtube\.com\/(?:(?:watch)?\?(?:.*&)?v(?:i)?=|(?:embed|v|vi|user)\/))([^\?&\"'<> #]+)/", $input, $matches);

  if (!empty($matches[1])) {
    $video_id = $matches[1];
    return $video_id;
  }

  return FALSE;
}

/**
 * Returns a list of standard YouTube video sizes.
 */
function youtube_size_options() {
  return array(
    'responsive' => 'responsive (full-width of container)',
    '420x315' => '450px by 315px',
    '480x360' => '480px by 360px',
    '640x480' => '640px by 480px',
    '960x720' => '960px by 720px',
    'custom' => 'custom',
  );
}

/**
 * Returns a list of thumbnail link types.
 */
function youtube_thumbnail_link_types() {
  $link_types = array(
    'content' => t('Content'),
    'youtube' => t('YouTube'),
  );

  if (module_exists('youtube_colorbox')) {
    $link_types['colorbox'] = t('Colorbox');
  }

  return $link_types;
}

/**
 * Splits height and width when given size, as from youtube_size_options.
 */
function youtube_get_dimensions($size = NULL, $width = NULL, $height = NULL) {
  $dimensions = array();
  if ($size == 'responsive') {
    $dimensions['width'] = '100%';
    $dimensions['height'] = '100%';
  }
  elseif ($size == 'custom') {
    $dimensions['width'] = strstr($width, '%') ? (int) $width . '%' : (int) $width;
    $dimensions['height'] = strstr($height, '%') ? (int) $height . '%' : (int) $height;
  }
  else {
    // Locate the 'x'.
    $strpos = strpos($size, 'x');
    // Width is the first dimension.
    $dimensions['width'] = substr($size, 0, $strpos);
    // Height is the second dimension.
    $dimensions['height'] = substr($size, $strpos + 1, strlen($size));
  }

  return $dimensions;
}

/**
 * Retreve youtube thumbnail image via YouTube API.
 *
 * TODO add error messaging if something goes wrong, and return FALSE.
 *
 * @param $id
 *   The video_id of the particular YouTube video.
 * @param $force_small
 *   When TRUE, this function should return the standard size image regardless
 *   of what the thumb_hires variable is set to. This is used should
 *   the high resolution image be found to not exist for a particular video.
 */
function youtube_get_remote_image($id = NULL, $force_small = FALSE) {
  // This variable is TRUE when higher resolution thumnbails should be saved.
  // The only thumbnail resolution higher than the standard 480 is
  // 'maxresdefault'. This resolution image is not guaranteed to exist. After
  // saving the file, we check to ensure that it does.
  $youtube_thumb_hires = config_get('youtube.settings', 'youtube_thumb_hires');

  if ($youtube_thumb_hires && !$force_small) {
    $src = youtube_build_remote_image_path($id, 'maxresdefault');
  }
  else {
    $src = youtube_build_remote_image_path($id);
  }

  // Make the actual request to download the file.
  $image_result = backdrop_http_request($src);

  // Assure the youtube thumbnail directory exists.
  $files = config_get('system.core', 'file_public_path');
  $youtube_dir = config_get('youtube.settings', 'youtube_thumb_dir');
  $youtube_path = $files . '/' . $youtube_dir;
  if (!file_prepare_directory($youtube_path, FILE_CREATE_DIRECTORY) && !mkdir($youtube_path, 0775, TRUE)) {
    watchdog('youtube', 'Failed to create YouTube thumbnail directory: %dir', array('%dir' => $youtube_path), WATCHDOG_ERROR);
  }

  // Save the file.
  $dest = $files . '/' . $youtube_dir . '/' . $id . '.png';
  file_put_contents($dest, $image_result->data);

  // If the high resolution image was saved but didn't actually exist, a very
  // small placeholder image from YouTube will have been saved. By checking the
  // dimensions of this image, we can determine if we saved the placeholder.
  if ($youtube_thumb_hires && !$force_small) {
    $image_path = $files . '/' . $youtube_dir . '/' . $id . '.png';
    if (file_exists($image_path)) {
      $image_size = getimagesize($image_path);
      if (empty($image_size[0]) || $image_size[0] < 480) {
        // We saved the placeholder. Re-run this function with $force_small
        // set to TRUE. This will give us the standard, guaranteed, thumbnail.
        youtube_get_remote_image($id, TRUE);
      }
    }
  }

  return TRUE;
}

/**
 * Submit callback; delete all existing thumbnail image files.
 *
 * @see youtube_settings_form()
 */
function youtube_thumb_delete_all($form, &$form_state) {
  $files = config_get('system.core', 'file_public_path');;
  $youtube_dir = config_get('youtube.settings', 'youtube_thumb_dir');
  $youtube_path = $files . '/' . $youtube_dir;

  $imagestyleflush = t('Derivatives generated with image styles were not
    deleted. <a href="@imagestyleflush">Image style flush</a> may help with that.',
    array('@imagestyleflush' => 'https://www.drupal.org/project/imagestyleflush'));

  if (file_prepare_directory($youtube_path) && file_unmanaged_delete_recursive($youtube_path)) {
    backdrop_set_message(t('All YouTube Field thumbnail image files have been
      deleted and will be redownloaded upon the next page load.'));
    backdrop_set_message($imagestyleflush);
  }
  else {
    backdrop_set_message(t('There were no thumbnails to delete.'), 'warning');
    backdrop_set_message($imagestyleflush, 'warning');
  }
}

/**
 * Get YouTube image path by building correctly formed URL.
 *
 * @param $video_id
 *   The ID of the video to grab the thumbnail from.
 * @param  $version
 *   Which version of the thumbnail to grab.
 * @return string
 *   The youtube.com image path to the specified version/video.
 */
function youtube_build_remote_image_path($video_id = NULL, $version = '0') {
  // The different versions of the image made available by YouTube.
  // http://stackoverflow.com/questions/2068344/how-to-get-thumbnail-of-youtube-video-link-using-youtube-api
  $versions = array(
    '0',
    'hqdefault',
    'mqdefault',
    'maxresdefault',
    'default',
    '1',
    '2',
    '3',
  );

  if (!$video_id || !in_array($version, $versions)) {
    return;
  }

  $version_path = 'http://img.youtube.com/vi/' . $video_id . '/' . $version . '.jpg';
  return url($version_path);
}

/**
 * Implements hook_feeds_processor_targets_alter().
 *
 * Adds a target option for YouTube fields to Feeds mapping options.
 *
 * @param &$targets
 *   Array containing the targets to be offered to the user. Add to this array
 *   to expose additional options. Remove from this array to suppress options.
 *   Remove with caution.
 * @param $entity_type
 *   The entity type of the target, for instance a 'node' entity.
 * @param $bundle_name
 *   The bundle name for which to alter targets.
 */
function youtube_feeds_processor_targets_alter(&$targets, $entity_type, $bundle_name) {
  foreach (field_info_instances($entity_type, $bundle_name) as $name => $instance) {
    $info = field_info_field($name);

    if (in_array($info['type'], array('youtube'))) {
      $targets[$name] = array(
        'name' => check_plain($instance['label']),
        'callback' => 'youtube_set_target',
        'description' => t('The @label field of the node.', array('@label' => $instance['label'])),
      );
    }
  }
}

/**
 * Callback to set the Feeds target for a YouTube field.
 *
 * @param $source
 *   Field mapper source settings.
 * @param $entity
 *   An entity object, for instance a node object.
 * @param $target
 *   A string identifying the target on the node.
 * @param $value
 *   The value to populate the target with.
 * @param $mapping
 *   Associative array of the mapping settings from the per mapping
 *   configuration form.
 */
function youtube_set_target($source, $entity, $target, $value, $mapping) {
  if (empty($value)) {
    return;
  }

  if (!is_array($value)) {
    $value = array($value);
  }

  $info = field_info_field($target);
  $field = isset($entity->$target) ? $entity->$target : array(LANGUAGE_NONE => array());

  // Allow for mappings to a multi-value field on the same target.
  $delta = count($field[LANGUAGE_NONE]);

  foreach ($value as $v) {
    if ($info['cardinality'] == $delta) {
      break;
    }

    if (is_object($v) && ($v instanceof FeedsElement)) {
      $v = $v->getValue();
    }

    if (is_scalar($v)) {
      $video_id = youtube_get_video_id($v);
      if ($video_id) {
        $entity->{$target}[LANGUAGE_NONE][$delta] = array(
          'input' => $v,
          'video_id' => $video_id,
        );
        $delta++;
      }
    }
  }
}
