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
  // Pull out facebook and put it back at the beginning.
  $face = $services['facebook'];
  unset($services['facebook']);
  $services = array('facebook' => $face)+$services;

  // Add an additional service.
  $services['viemo'] = array(
    'name' => 'Viemo'
    'fa-icon' => 'fa-vimeo-square',
  );
}

/**
 * Alter the links displayedby on_the_web_display_block().
 *
 * This alter function can be used to:
 * - Change how the links are rendered.
 *
 * @see on_the_web_display_block().
 */
function hook_on_the_web_links_alter(&$links) {
  // Turn the links into an item-list.
  $items = array();
  foreach ($links as $link) {
    $items[] = backdrop_render($link);
  }
  $links = array(
    '#theme' => 'item_list',
    '#items' => $items,
  );
}

