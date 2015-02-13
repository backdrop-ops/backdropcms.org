<?php
/**
 * @file
 * All theme functions for On The Web.
 */

/**
 * Theme function
 *
 * @param $service
 *    Icon for appropriate service.
 * @param $link
 *    URL where link should point.
 * @param $title
 *    Title attribute for the link tag.
 *
 * @return
 *    Linked icon with wrapper markup.
 */
function theme_on_the_web_item($variables) {
  $service = $variables['service'];
  $link = $variables['link'];
  $title = $variables['title'];
  $classes = $variables['classes'];

  $config = config('on_the_web.settings');
  $type = $config->get('type');
  $target = $config->get('target');

  // Determine attributes for the link
  $attributes  = array(
    'class' => $classes,
    'title' => $title,
    'rel' => 'nofollow',
  );
  if ($target == TRUE) {
    $attributes['target'] = '_blank';
  }
  $options = array('attributes' => $attributes);

  if ($type == 'image') {
    $inner = theme('on_the_web_image', array('service' => $service, 'title' => $title));
    $options['html'] = TRUE;
  }
  else {
    $inner = $title;
  }

  // Link the image and wrap it in a span.
  $linked_image = l($inner, $link, $options);

  return $linked_image;
}

/**
 * Theme function
 *
 * @param $service
 *    Icon for appropriate service.
 * @param $title
 *    Title attribute for the link tag.
 *
 * @return 
 *    Icon image of appropriate size.
 */
function theme_on_the_web_image($variables) {
  $service = $variables['service'];
  $title = $variables['title'];

  // Get the size.
  $size = config_get('on_the_web.settings', 'size');

  // Assemble necessary variables for building the image.
  $variables = array(
    'uri' => backdrop_get_path('module', 'on_the_web') . '/images/' . $size . '/' . $service . '.png', 
    'alt' => $title,
    'title' => $title,
  );

  // Build the img tag.
  $image = theme('image', $variables);

  return $image;
}

/**
 * Theme the drag-and-drop form.
 *
 * Arranges records in a table, and adds the css and js for draggable sorting.
 *
 * @ingroup themeable
 * @ingroup forms
 */
function theme_on_the_web_settings_form($variables) {
  $form = $variables['form'];
  $rows = array();
  $disabled_rows = array();

  foreach (element_children($form['services'], TRUE) as $service) {
    $row = array();

    $row[] = backdrop_render($form['services'][$service]['title']);
    $row[] = backdrop_render($form['services'][$service]['page']);

    // Now, render the weight row.
    $form['services'][$service]['weight']['#attributes']['class'][] = 'icon-weight';
    $row[] = backdrop_render($form['services'][$service]['weight']);

    // Add this row to our collection of rows, and give it the 'draggable' class.
    $rows[] = array(
      'data' => $row,
      'class' => array('draggable'),
    );
  }

  $output = '';
  if (count($rows)) {
    $header = array(t('Icon'), t('URL'), t('Weight'));
    $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'on-the-web-settings-table')));
  }

  // Pull out the submit button.
  $submit = backdrop_render($form['submit']);
  // Add the rest of the form elements on top.
  $output = backdrop_render_children($form) . $output . $submit;

  backdrop_add_tabledrag('on-the-web-settings-table', 'order', 'self', 'icon-weight');
  backdrop_add_css(backdrop_get_path('module', 'on_the_web') . '/css/on_the_web.admin.css');

  return $output;
}
