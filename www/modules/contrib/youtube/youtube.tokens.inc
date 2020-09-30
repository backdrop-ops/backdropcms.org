<?php
/**
 * @file
 * YouTube module token support.
 */

/**
 * Implements hook_token_info_alter().
 *
 * Alters and adds tokens for each youtube field.
 *
 * @param $data
 *   The associative array of token definitions from hook_token_info().
 */
function youtube_token_info_alter(&$data) {
  // Get all youtube fields. Gather entity_type and bundle information.
  $fields = field_info_fields();
  $youtube_fields = array();
  foreach ($fields as $name => $field) {
    if ($field['type'] == 'youtube') {
      foreach ($field['bundles'] as $type => $entity_type) {
        foreach ($entity_type as $bundle) {
          $youtube_fields[] = array(
            'entity_type' => $type,
            'bundle' => $bundle,
            'field_name' => $name,
          );
        }
      }
    }
  }

  foreach ($youtube_fields as $field) {
    if (isset($data['tokens'][$field['entity_type']])) {
      $field_info = field_info_instance($field['entity_type'], $field['field_name'], $field['bundle']);
      $field_label = $field_info['label'];
  
      // Modify the default field token.
      $data['tokens'][$field['entity_type']][$field['field_name']] = array(
        'name' => $field_label . t(": Default"),
        'description' => t("The YouTube video field value's Default (or Token if exists) view mode output."),
      );
  
      // Add two new tokens.
      $data['tokens'][$field['entity_type']][$field['field_name'] . '__youtube_video_url'] = array(
        'name' => $field_label . t(": Video URL"),
        'description' => t("The YouTube video field value's youtube.com URL."),
      );
      $data['tokens'][$field['entity_type']][$field['field_name'] . '__youtube_image_url'] = array(
        'name' => $field_label . t(": Image URL"),
        'description' => t("The YouTube video field value's local image URL."),
      );
    }
  }
}

/**
 * Provide replacement values for placeholder tokens.
 *
 * Replaces youtube_video_url and youtube_image_url tokens.
 *
 * @param $type
 *   The machine-readable name of the type (group) of token being replaced, such
 *   as 'node', 'user', or another type defined by a hook_token_info()
 *   implementation.
 * @param $tokens
 *   An array of tokens to be replaced. The keys are the machine-readable token
 *   names, and the values are the raw [type:token] strings that appeared in the
 *   original text.
 * @param $data
 *   (optional) An associative array of data objects to be used when generating
 *   replacement values, as supplied in the $data parameter to token_replace().
 * @param $options
 *   (optional) An associative array of options for token replacement; see
 *   token_replace() for possible values.
 *
 * @return
 *   An associative array of replacement values, keyed by the raw [type:token]
 *   strings from the original text.
 *
 * @see youtube_tokens_info_alter()
 */
function youtube_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $url_options = array('absolute' => TRUE);
  if (isset($options['language'])) {
    $url_options['language'] = $options['language'];
    $language_code = $options['language']->langcode;
  }
  else {
    $language_code = NULL;
  }
  $sanitize = !empty($options['sanitize']);

  $replacements = array();

  if ($type == 'node' && !empty($data['node'])) {
    $node = $data['node'];

    foreach ($tokens as $name => $original) {
      if (!strpos($name, '__youtube_')) {
        // This isn't a youtube token!
        continue;
      }

      $token_pieces = explode('__', $name);
      if (count($token_pieces) != 2) {
        continue;
      }

      $field_name = $token_pieces[0];
      $token_name = $token_pieces[1];

      switch ($token_name) {
        case 'youtube_video_url':
          $field = $node->$field_name;
          $video_id = $field[LANGUAGE_NONE][0]['video_id'];
          $replacements[$original] = 'http://www.youtube.com/watch?v=' . $video_id;
          break;

        case 'youtube_image_url':
          global $base_url;
          global $base_path;
          $field = $node->$field_name;
          $video_id = $field[LANGUAGE_NONE][0]['video_id'];
          $file_path = config_get('system.core', 'file_public_path') . '/';
          $file_path .= config_get('youtube.settings', 'youtube_thumb_dir');
          $file_path .= '/' . $video_id . '.png';
          $full_path = $base_url . $base_path . $file_path;
          if (!file_exists($full_path)) {
            youtube_get_remote_image($video_id);
          }
          $replacements[$original] = $full_path;
          break;
      }
    }
  }

  return $replacements;
}
