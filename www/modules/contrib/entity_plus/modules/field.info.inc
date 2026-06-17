<?php

/**
 * @file
 * Provides info for fields.
 */

/**
 * Implements hook_entity_property_info() on top of field module.
 *
 * @see entity_plus_field_info_alter()
 * @see entity_plus_entity_property_info()
 */
function entity_plus_metadata_field_entity_property_info() {
  $info = array();
  // Loop over all field instances and add them as property.
  foreach (field_info_fields() as $field_name => $field) {
    $field += array('bundles' => array());
    if ($field_type = field_info_field_types($field['type'])) {
      // Add in our default callback as the first one.
      $field_type += array('property_callbacks' => array());
      array_unshift($field_type['property_callbacks'], 'entity_plus_metadata_field_default_property_callback');

      foreach ($field['bundles'] as $entity_type => $bundles) {
        foreach ($bundles as $bundle) {
          $instance = field_info_instance($entity_type, $field_name, $bundle);

          if ($instance && empty($instance['deleted'])) {
            foreach ($field_type['property_callbacks'] as $callback) {
              if (function_exists($callback)) {
                $callback($info, $entity_type, $field, $instance, $field_type);
              }
              else {
                watchdog($entity_type, 'Missing property callback %callback.', array('%callback' => $callback), WATCHDOG_ERROR);
              }
            }
          }
        }
      }
    }
  }
  return $info;
}

/**
 * Callback to add in property info defaults per field instance.
 * @see entity_plus_metadata_field_entity_property_info()
 */
function entity_plus_metadata_field_default_property_callback(&$info, $entity_type, $field, $instance, $field_type) {
  if (!empty($field_type['property_type'])) {
    if ($field['cardinality'] != 1) {
      $field_type['property_type'] = 'list<' . $field_type['property_type'] . '>';
    }
    // Add in instance specific property info, if given and apply defaults.
    $name = $field['field_name'];
    $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$name];
    $instance += array('property info' => array());
    $property = $instance['property info'] + array(
      // Since the label will be exposed via hook_token_info() and it is not
      // clearly defined if that should be sanitized already we prevent XSS
      // right here (field labels are user provided text).
      'label' => filter_xss_admin($instance['label']),
      'type' => $field_type['property_type'],
      'description' => t('Field "@name".', array('@name' => $name)),
      'getter callback' => 'entity_plus_metadata_field_property_get',
      'setter callback' => 'entity_plus_metadata_field_property_set',
      'access callback' => 'entity_plus_metadata_field_access_callback',
      'query callback' => 'entity_plus_metadata_field_query',
      'translatable' => !empty($field['translatable']),
      // Specify that this property stems from a field.
      'field' => TRUE,
      'required' => !empty($instance['required']),
    );
    // For field types of the list module add in the options list callback.
    if (strpos($field['type'], 'list') === 0) {
      $property['options list'] = 'entity_plus_metadata_field_options_list';
    }
  }
}

/**
 * Additional callback to adapt the property info for text fields. If a text field is processed we make use of a separate data structure so that format filters are available too. For the text value the sanitized, thus processed value is returned by default.
 *
 * @see entity_plus_metadata_field_entity_property_info()
 * @see entity_plus_field_info_alter()
 * @see entity_plus_property_text_formatted_info()
 */
function entity_plus_metadata_field_text_property_callback(&$info, $entity_type, $field, $instance, $field_type) {
  if (!empty($instance['settings']['text_processing']) || $field['type'] == 'text_with_summary') {
    // Define a data structure for dealing with text that is formatted or has
    // a summary.
    $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$field['field_name']];

    $property['getter callback'] = 'entity_plus_metadata_field_verbatim_get';
    $property['setter callback'] = 'entity_plus_metadata_field_verbatim_set';
    unset($property['query callback']);

    if (empty($instance['settings']['text_processing'])) {
      $property['property info'] = entity_plus_property_field_item_textsummary_info();
    }
    else {
      // For formatted text we use the type name 'text_formatted'.
      $property['type'] = ($field['cardinality'] != 1) ? 'list<text_formatted>' : 'text_formatted';
      $property['property info'] = entity_plus_property_text_formatted_info();
    }
    // Enable auto-creation of the item, so that it is possible to just set
    // the textual or summary value.
    $property['auto creation'] = 'entity_plus_property_create_array';

    if ($field['type'] != 'text_with_summary') {
      unset($property['property info']['summary']);
    }
  }
}

/**
 * Additional callback to adapt the property info for term reference fields.
 * @see entity_plus_metadata_field_entity_property_info()
 */
function entity_plus_metadata_field_term_reference_callback(&$info, $entity_type, $field, $instance, $field_type) {
  $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$field['field_name']];
  if (count($field['settings']['allowed_values']) == 1) {
    $settings = reset($field['settings']['allowed_values']);
    $property['bundle'] = $settings['vocabulary'];
  }
  // Only add the options list callback for controlled vocabularies, thus
  // vocabularies not using the autocomplete widget.
  if ($instance['widget']['type'] != 'taxonomy_autocomplete') {
    $property['options list'] = 'entity_plus_metadata_field_options_list';
  }
}

/**
 * Additional callback to adapt the property info for file fields.
 * @see entity_plus_metadata_field_entity_property_info()
 */
function entity_plus_metadata_field_file_callback(&$info, $entity_type, $field, $instance, $field_type) {
  $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$field['field_name']];
  // Define a data structure so it's possible to deal with files and their
  // descriptions.
  $property['getter callback'] = 'entity_plus_metadata_field_verbatim_get';
  $property['setter callback'] = 'entity_plus_metadata_field_verbatim_set';

  // Auto-create the field $items as soon as a property is set.
  $property['auto creation'] = 'entity_plus_metadata_field_file_create_item';
  $property['validation callback'] = 'entity_plus_metadata_field_file_validate_item';

  $property['property info'] = entity_plus_property_field_item_file_info();

  if (empty($instance['settings']['description_field'])) {
    unset($property['property info']['description']);
  }
  if (empty($field['settings']['display_field'])) {
    unset($property['property info']['display']);
  }
  unset($property['query callback']);
}

/**
 * Additional callback to adapt the property info for image fields. This callback gets invoked after entity_plus_metadata_field_file_callback().
 *
 * @see entity_plus_metadata_field_entity_property_info()
 */
function entity_plus_metadata_field_image_callback(&$info, $entity_type, $field, $instance, $field_type) {
  $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$field['field_name']];
  // Update the property info with the info for image fields.
  $property['property info'] = entity_plus_property_field_item_image_info();

  if (empty($instance['settings']['alt_field'])) {
    unset($property['property info']['alt']);
  }
  if (empty($instance['settings']['title_field'])) {
    unset($property['property info']['title']);
  }
}

/**
 * Callback to alter the property info of date fields.
 *
 * @see entity_plus_metadata_field_entity_property_info()
 */
function entity_plus_metadata_field_date_callback(&$info, $entity_type, $field, $instance, $field_type) {
  $name = $field['field_name'];
  $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$name];

  if ($field['type'] != 'datestamp' || $field['settings']['timezone_db'] != 'UTC') {
    // Add a getter callback to convert the date into the right format.
    $property['getter callback'] = 'entity_plus_metadata_field_date_getter';
    $property['setter callback'] = 'entity_plus_metadata_field_date_setter';
    unset($property['query callback']);
  }
  if (!empty($field['settings']['todate'])) {
    // Define a simple data structure containing both dates.
    $property['type'] = ($field['cardinality'] != 1) ? 'list<struct>' : 'struct';
    $property['auto creation'] = 'entity_plus_metadata_date_struct_create';
    $property['getter callback'] = 'entity_plus_metadata_field_verbatim_get';
    $property['setter callback'] = 'entity_plus_metadata_field_verbatim_set';
    $property['property info'] = array(
      'value' => array(
        'type' => 'date',
        'label' => t('Start date'),
        'getter callback' => 'entity_plus_metadata_date_struct_getter',
        'setter callback' => 'entity_plus_metadata_date_struct_setter',
        // The getter and setter callbacks for 'value' and 'value2'
        // will not provide the field name as $name, we'll add it to $info.
        'field_name' => $field['field_name'],
        // Alert Microdata module that this value can be exposed in microdata.
        'microdata' => TRUE,
      ),
      'value2' => array(
        'type' => 'date',
        'label' => t('End date'),
        'getter callback' => 'entity_plus_metadata_date_struct_getter',
        'setter callback' => 'entity_plus_metadata_date_struct_setter',
        // The getter and setter callbacks for 'value' and 'value2'
        // will not provide the field name as $name, we'll add it to $info.
        'field_name' => $field['field_name'],
        // Alert Microdata module that this value can be exposed in microdata.
        // While microdata has not been ported to Backdrop, leave this here just in case.
        'microdata' => TRUE,
      ),
      'duration' => array(
        'type' => 'duration',
        'label' => t('Duration'),
        'desription' => t('The duration of the time period given by the dates.'),
        'getter callback' => 'entity_plus_metadata_date_duration_getter',
        // No setter callback for duration.
        // The getter callback for duration will not provide the field name
        // as $name, we'll add it to $info.
        'field_name' => $field['field_name'],
      ),
    );
    unset($property['query callback']);
  }
  else {
    // If this doesn't have a todate, it is handled as a date rather than a
    // struct. Enable microdata on the field itself rather than the properties.
    // While microdata has not been ported to Backdrop, leave this here just in case.
    $property['microdata'] = TRUE;
  }
}

/**
 * Callback to alter the property info of link fields.
 *
 * @see entity_plus_metadata_field_entity_property_info()
 */
function entity_plus_metadata_field_link_callback(&$info, $entity_type, $field, $instance, $field_type) {
  $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$field['field_name']];
  // Define a data structure so it's possible to deal with both the link title
  // and URL.
  $property['getter callback'] = 'entity_plus_metadata_field_verbatim_get';
  $property['setter callback'] = 'entity_plus_metadata_field_verbatim_set';

  // Auto-create the field item as soon as a property is set.
  $property['auto creation'] = 'entity_plus_metadata_field_link_create_item';

  $property['property info'] = array(
    'title' => array(
      'type' => 'text',
      'label' => t('The title of the link.'),
      'setter callback' => 'entity_plus_property_verbatim_set',
      'required' => $instance['required'] && ($instance['settings']['title'] == 'required'),
    ),
    'url' => array(
      'type' => 'text',
      'label' => t('The URL of the link.'),
      'setter callback' => 'entity_plus_property_verbatim_set',
      'getter callback' => 'entity_plus_link_url_property_get',
      'required' => $instance['required'] && !$instance['settings']['url'],
    ),
    'attributes' => array(
      'type' => 'struct',
      'label' => t('The attributes of the link.'),
      'setter callback' => 'entity_plus_property_verbatim_set',
      'getter callback' => 'entity_plus_link_attribute_property_get',
    ),
    'display_url' => array(
      'type' => 'uri',
      'label' => t('The full URL of the link.'),
      'setter callback' => 'entity_plus_property_verbatim_set',
    ),
  );
  if ($instance['settings']['title'] == 'none') {
    unset($property['property info']['title']);
  }
  unset($property['query callback']);
}
