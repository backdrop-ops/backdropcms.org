<?php
/**
 * @file
 * Theme function overrides.
 */

/*******************************************************************************
 * Preprocess functions: prepare variables for templates.
 ******************************************************************************/

/**
 * Prepares variables for page.tpl.php
 */
function borg_preprocess_page() {
  // Add the Source Sans Pro font.
  backdrop_add_css('https://fonts.googleapis.com/css?family=Source+Sans+Pro:300,400,600,700', array('type' => 'external'));
  // Add FontAwesome.
  backdrop_add_css('https://maxcdn.bootstrapcdn.com/font-awesome/4.3.0/css/font-awesome.min.css', array('type' => 'external'));

  // Add Flexslider to the front page only.
  if (backdrop_is_front_page()) {
    $path = backdrop_get_path('theme', 'borg');
    backdrop_add_css($path . '/css/flexslider.css');
    backdrop_add_js($path . '/js/jquery.flexslider.js');
    $script = "
$(window).load(function() {
  $('.flexslider').flexslider();
});";
    backdrop_add_js($script, array('type' => 'inline'));
  }
}

/**
 * Prepares variables for layout.tpl.php
 */
function borg_preprocess_layout__double_fixed(&$variables) {
  $node = menu_get_object();
  if (isset($node) && isset($node->type) && $node->type === 'book') {
    $variables['classes'][] = 'drawer-open';
  }
  else {
    $variables['classes'][] = 'drawer-closed';
    $array_key = array_search('layout-both-sidebars', $variables['classes']);
    if ($array_key !== FALSE) {
      $variables['classes'][$array_key] = 'layout-one-sidebar';
    }
  }
}

/*******************************************************************************
 * Theme function overrides.
 ******************************************************************************/

/**
 * Overrides theme_menu_link().
 */
function borg_menu_link(array $variables) {
  $element = $variables['element'];
  $sub_menu = '';

  if ($element['#below']) {
    $sub_menu = backdrop_render($element['#below']);
  }

  $menu_name = isset($element['#original_link']['menu_name']) ? $element['#original_link']['menu_name'] : NULL;
  if ($menu_name === 'main-menu' || $menu_name === 'menu-handbook') {
    // Add the font awesome icon.
    if ($element['#href']) {
      $element['#title'] .= ' <i class="fa fa-forward fa-fw"></i>';
      $element['#localized_options']['html'] = TRUE;
    }

    // If this is the handbook link and we're on a book page, set an active class.
    if ($element['#href'] === 'node/1') {
      $node = menu_get_object();
      if (isset($node) && isset($node->type) && $node->type === 'book') {
        $element['#attributes']['class'][] = 'active';
      }
    }
  }

  $output = l($element['#title'], $element['#href'], $element['#localized_options']);
  return '<li' . backdrop_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}

/**
 * Overrides theme_on_the_web_image().
 */
function borg_on_the_web_image($variables) {
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
