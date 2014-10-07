<?php

/**
 * @file
 */

/**
 * Preprocess functions.
 */

/**
 * Prepares variables for html.tpl.php
 */
function backdropcms_preprocess_html() {
  drupal_add_css('https://fonts.googleapis.com/css?family=Open+Sans:400italic,400,700', array('type' => 'external'));
}

/**
 * Prepares variables for page.tpl.php
 */
function backdropcms_preprocess_page(&$variables) {
  $variables['wrapper_classes'] = array();
  $variables['drawer_classes'] = array();

  $current_menu_item = $_GET['q'];
  if (isset($variables['node']) && $variables['node']->type === 'book') {
    $variables['wrapper_classes'][] = 'drawer-open';
    $variables['drawer_classes'][] = 'open';
    menu_set_active_item('node/1');
  }

  // Note that the active menu item determines the active class when rendering
  // this menu.
  $variables['main_menu'] = menu_tree('main-menu');
  foreach (element_children($variables['main_menu']) as $key) {
    $item = &$variables['main_menu'][$key];
    // Populate font awesome glyphs next to menu items.
    $item['#title'] = check_plain($variables['main_menu'][$key]['#title']) . ' <i class="fa fa-chevron-right fa-fw"></i>';
    $item['#localized_options']['html'] = TRUE;

    // Attempt to standardize active classes.
    $trail_class = array_search('active-trail', $item['#attributes']['class']);
    if ($trail_class !== FALSE) {
      $item['#attributes']['class'][$trail_class] = 'active';
    }
    if ($variables['is_front'] && $item['#href'] === '<front>') {
      $item['#attributes']['class'][] = 'active';
    }
    $item['#attributes']['class'][] = 'menu-' . $key;

  }
  $variables['main_menu'] = drupal_render($variables['main_menu']);

  // Restore any current menu item.
  menu_set_active_item($current_menu_item);

  // Render the handbook menu.
  $handbook_tree = menu_tree('menu-handbook');
  $variables['handbook_menu'] = drupal_render($handbook_tree);

  // Compact custom classes to strings.
  $variables['wrapper_classes'] = implode(' ', $variables['wrapper_classes']);
  $variables['drawer_classes'] = implode(' ', $variables['drawer_classes']);
}

/**
 * Prepares variables for block.tpl.php
 */
function backdropcms_preprocess_block(&$variables) {

}

/**
 * Theme overrides.
 */

/**
 * Overrides theme_on_the_web_image().
 */
function backdropcms_on_the_web_image($variables) {
  if ($variables['service'] == 'twitter') {
    return '<i class="fa fa-twitter-square"></i>';
  }
  if ($variables['service'] == 'facebook') {
    return '<i class="fa fa-facebook-square"></i>';
  }
  if ($variables['service'] == 'google') {
    return '<i class="fa fa-google-plus-square"></i>';
  }
  if ($variables['service'] == 'youtube') {
    return '<i class="fa fa-youtube-square"></i>';
  }
}
