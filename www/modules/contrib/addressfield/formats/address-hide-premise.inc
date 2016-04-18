<?php
/**
 * @file
 * Hide the 2nd street address (premise) field.
 */

/**
 * Format callback.
 *
 * @see CALLBACK_addressfield_format_callback()
 */
function addressfield_format_address_hide_premise(&$format, $address, $context = array()) {
  if (isset($format['street_block']['premise'])) {
    $format['street_block']['premise']['#access'] = FALSE;
  }
}
