<?php

/**
 * @file
 * YouTube Field support for use with the migrate module.
 */

/**
 * Implements hook_migrate_api().
 */
function youtube_migrate_api() {
  $api = array(
    'api' => 2,
    'field handlers' => array('MigrateYoutubeFieldHandler'),
  );
  return $api;
}

class MigrateYoutubeFieldHandler extends MigrateFieldHandler {

  /**
   * Declares the type(s) of fields used.
   */
  public function __construct() {
    $this->registerTypes(array('youtube'));
  }

  /**
   * Arguments for a YouTube field migration.
   *
   * @param string $input
   *   The URL of the YouTube video. If a value is not supplied, this will be
   *   constructed from the $video_id.
   *
   * @return array
   *   An array of the defined variables in this scope.
   */
  static function arguments($input = NULL) {
    return get_defined_vars();
  }

  /**
   * Implementation of MigrateFieldHandler::fields().
   *
   * @param $type
   *   The field type.
   * @param $instance
   *   Instance info for the field.
   * @param Migration $migration
   *   The migration context for the parent field. We can look at the mappings
   *   and determine which subfields are relevant.
   * @return array
   */
  public function fields($type, $instance, $migration = NULL) {
    return array(
      'input' => t('Subfield: The full YouTube video URL'),
    );
  }

  /**
   * Converts incoming data to the proper format for YouTube fields.
   *
   * @param object $entity
   *   The destination entity which will hold the field arrays.
   * @param array $field_info
   *   Metadata for the YouTube field being populated.
   * @param array $instance
   *   Metadata for this instance of the YouTube field being populated.
   * @param array $values
   *   Array of YouTube values to be fielded.
   *
   * @return array|null
   *   An array of YouTube fields.
   */
  public function prepare($entity, array $field_info, array $instance, array $values) {
    if (isset($values['arguments'])) {
      $arguments = $values['arguments'];
      unset($values['arguments']);
    }
    else {
      $arguments = array();
    }

    $language = $this->getFieldLanguage($entity, $field_info, $arguments);

    $values = array_filter($values);
    foreach ($values as $delta => $value) {
      $item = array();

      $video_id = youtube_get_video_id($value);
      if (!empty($video_id)) {
        $item['input'] = $value;
        $item['video_id'] = $video_id;
      }

      $return[$language][$delta] = $item;
    }

    return isset($return) ? $return : NULL;
  }
}
