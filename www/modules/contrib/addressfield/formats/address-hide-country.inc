<?php
/**
 * @file
 * Hide the country when only one country is available.
 */

/**
 * Format callback.
 *
 * @see CALLBACK_addressfield_format_callback()
 */
function addressfield_format_address_hide_country(&$format, $address, $context = array()) {
  if ($context['mode'] == 'form') {
    // In form mode we hide when the number of options available is one.
    if (!empty($format['country']['#options']) && count($format['country']['#options']) == 1) {
      $format['country']['#access'] =  FALSE;
    }
  }
  elseif ($context['mode'] == 'render') {
    // In render mode the element does not have an #options list so we look in
    // the field instance settings if given. If we find a single country option
    // and it matches the country of the current address, hide it.
    if (!empty($context['instance']['widget']['settings']['available_countries']) &&
      count($context['instance']['widget']['settings']['available_countries']) == 1) {
      if (isset($context['instance']['widget']['settings']['available_countries'][$address['country']])) {
        $format['country']['#access'] = FALSE;
      }
    }
  }
}
