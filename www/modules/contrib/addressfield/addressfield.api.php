<?php
/**
 * @file
 * API documentation for Addressfield.
 */

/**
 * Info hook defining available address formats.
 */
function hook_addressfield_format_info() {
  return array(
    'address' => array(
      'title' => t('Address form (country-specific)'),
      'format callback' => 'addressfield_format_address_generate',
      'type' => 'address',
      'weight' => -100,
      'file' => 'formats/address.inc',
    ),
  );
}

/**
 * Format generation callback.
 *
 * @param $format
 *   The address format being generated.
 * @param $address
 *   The address this format is generated for.
 * @param $context
 *   An associative array of context information pertaining to how the address
 *   format should be generated. If no mode is given, it will initialize to the
 *   default value. The remaining context keys should only be present when the
 *   address format is being generated for a field:
 *   - mode: either 'form' or 'render'; defaults to 'render'.
 *   - field: the field info array.
 *   - instance: the field instance array.
 *   - langcode: the langcode of the language the field is being rendered in.
 *   - delta: the delta value of the given address.
 *
 * @ingroup addressfield_format
 */
function CALLBACK_addressfield_format_callback(&$format, $address, $context = array()) {
  // No example.
}

/**
 * Allow other modules to alter at run time which handlers to use.
 * Useful when you want to conditionally add/remove handlers based on data
 * stored on an entity.
 *
 * @param &$handlers
 *    Array of handlers that can be used. Use a FALSE value to indicate it
 *    shouldn't be used and the name to indicate it should be used to generate
 *    the address.
 * @param $address
 *    The address data used for the form
 * @param $context
 *   An array of context arguments:
 *     - 'mode': can be either 'form' or 'render'
 *     - (optional) 'field': when generated for a field, the field
 *     - (optional) 'instance': when generated for a field, the field instance
 *     - (optional) 'langcode': when generated for a field, the langcode
 *       this field is being rendered in.
 *     - (optional) 'delta': when generated for a field, the delta of the
 *       currently handled address.
 *     - (optional) 'entity': The entity that the form/display is created for
 *     - (optional) 'entity_type': The entity_type of the entity provided
 *     - (optional) 'entity_types': If an entity_type can't be established,
 *       an array of types is passed instead.
 *
 */
function hook_addressfield_handlers_alter(&$handlers, $address, $context) {
  // No example.
}

/**
 * Allows modules to alter the default values for an address field.
 *
 * @param $default_values
 *   The array of default values. The country is populated from the
 *   'default_country' widget setting.
 * @param $context
 *   An array with the following keys:
 *   - field: The field array.
 *   - instance: The instance array.
 *   - address: The current address values. Allows for per-country defaults.
 */
function hook_addressfield_default_values_alter(&$default_values, $context) {
  // If no other default country was provided, set it to France.
  // Note: you might want to check $context['instance']['required'] and
  // skip setting the default country if the field is optional.
  if (empty($default_values['country'])) {
    $default_values['country'] = 'FR';
  }

  // Determine the country for which other defaults should be provided.
  $selected_country = $default_values['country'];
  if (isset($context['address']['country'])) {
    $selected_country = $context['address']['country'];
  }

  // Add defaults for the US.
  if ($selected_country == 'US') {
    $default_values['locality'] = 'New York';
    $default_values['administrative_area'] = 'NY';
  }
}

/**
 * Allows modules to alter the predefined address formats.
 *
 * @param $address_formats
 *   The array of all predefined address formats.
 *
 * @see addressfield_get_address_format()
 */
function hook_addressfield_address_formats_alter(&$address_formats) {
  // Remove the postal_code from the list of required fields for China.
  $address_formats['CN']['required_fields'] = array('locality', 'administrative_area');
}

/**
 * Allows modules to alter the predefined administrative areas.
 *
 * @param $administrative_areas
 *   The array of all predefined administrative areas.
 *
 * @see addressfield_get_administrative_areas()
 */
function hook_addressfield_administrative_areas_alter(&$administrative_areas) {
  // Alter the label of the Spanish administrative area with the iso code PM.
  $administrative_areas['ES']['PM'] = t('Balears / Baleares');

  // Add administrative areas for imaginary country XT, keyed by their
  // imaginary ISO codes.
  $administrative_areas['XT'] = array(
      'A' => t('Aland'),
      'B' => t('Bland'),
  );
}

/**
 * Allows modules to add arbitrary AJAX commands to the array returned from the
 * standard address field widget refresh.
 *
 * @param &$commands
 *   The array of AJAX commands used to refresh the address field widget.
 * @param $form
 *   The rebuilt form array.
 * @param $form_state
 *   The form state array from the form.
 *
 * @see addressfield_standard_widget_refresh()
 */
function hook_addressfield_standard_widget_refresh_alter(&$commands, $form, $form_state) {
  // Display an alert message.
  $commands[] = ajax_command_alert(t('The address field widget has been updated.'));
}
