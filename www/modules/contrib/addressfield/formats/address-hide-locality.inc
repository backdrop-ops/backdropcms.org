<?php
/**
 * @file
 * Hide the locality field.
 */

/**
 * Format callback.
 *
 * @see CALLBACK_addressfield_format_callback()
 */
function addressfield_format_address_hide_locality(&$format, $address, $context = array()) {
  if (isset($format['locality_block']['administrative_area'])) {
    $format['locality_block']['locality']['#access'] = FALSE;
  }
}
