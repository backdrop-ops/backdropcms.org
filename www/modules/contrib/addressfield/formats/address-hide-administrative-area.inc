<?php
/**
 * @file
 * Hide the administrative area field.
 */

/**
 * Format callback.
 *
 * @see CALLBACK_addressfield_format_callback()
 */
function addressfield_format_address_hide_administrative_area(&$format, $address, $context = array()) {
  if (isset($format['locality_block']['administrative_area'])) {
    $format['locality_block']['administrative_area']['#access'] = FALSE;
  }
}
