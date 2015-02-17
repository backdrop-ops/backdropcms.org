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
  drupal_add_css('https://fonts.googleapis.com/css?family=Source+Sans+Pro:300,400,700', array('type' => 'external'));
  // Add FontAwesome.
  drupal_add_css('https://maxcdn.bootstrapcdn.com/font-awesome/4.3.0/css/font-awesome.min.css', array('type' => 'external'));
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

  // Add the font awesome icon.
  if ($element['#href']) {
    $element['#title'] .= ' <i class="fa fa-forward fa-fw"></i>';
    $element['#localized_options']['html'] = TRUE;
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
