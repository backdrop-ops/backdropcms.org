<?php
/**
 * @file
 * API documentation for On The Web.
 */

/**
 * Alter the services returned by on_the_web_get_services().
 *
 * This alter function can be used to:
 * - Change the order of links in the list of services.
 * - Add additional services.
 *
 * @see on_the_web_get_services().
 */
function hook_on_the_web_get_services_alter(&$services) {
  // Pull out facebook.
  $face = $services['facebook'];
  // Remove facebook.
  unset($services['facebook']);
  // Put facebook at the begining.
  $services = array('facebook' => $face)+$services;

  // Add an additional service.
  $services['soundcloud'] = array('name' => 'SoundCloud');
}
