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
  $icon = $variables['icon'];
  $size = $variables['size'];
  $title = $variables['title'];
  $link_classes = $variables['classes'];
  $icon_classes = $variables['icon_classes'];

  $config = config('on_the_web.settings');
  $type = $config->get('type');
  $target = $config->get('target');

  // Determine attributes for the link
  $attributes = array(
    'class' => $link_classes,
    'title' => $title,
    'aria-label' => $title,
    'rel' => 'nofollow',
  );
  if ($target == TRUE) {
    $attributes['target'] = '_blank';
    $attributes['aria-label'] .= ' (' . t('opens in new window') . ')';
  }

  $text = '';
  if ($type == 'font') {
    // Add the font awesome icon classes with support for v5.
    $icon_classes[] = $icon;
    $icon_classes[] = 'fa-fw';

    if ((!module_exists('font_awesome') && $config->get('version') == '5.x.x')
       || (module_exists('font_awesome') && config_get('font_awesome.settings', 'fontawesome') == 'v5')) {

      if (!in_array('fas', $icon_classes)) {
        $icon_classes[] = 'fab';
      }
    }
    else {
      $icon_classes[] = 'fa';
    }

    // Add the font awesome size classes.
    if ($size == 'lg') {
      $icon_classes[] = 'fa-3x';
    }
    else {
      $icon_classes[] = 'fa-2x';
    }

    $text = '<i aria-hidden="true" class="' . implode(' ', $icon_classes) . '"></i>';
  }

  elseif ($type == 'image') {
    $text = '<img src="' . $icon . '" />';
  }

  elseif ($type == 'anchor') {
    $style = 'background: transparent url(' . $icon . ') no-repeat top left;';
    $attributes['style'] = $style;
  }

  $options = array('attributes' => $attributes, 'html' => TRUE);
  return l($text, $link, $options);
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
  $version = config_get('on_the_web.settings', 'version');

  foreach (element_children($form['services'], TRUE) as $service) {
    $row = array();

    $row[] = backdrop_render($form['services'][$service]['title']);
    $row[] = backdrop_render($form['services'][$service]['page']);
    $row[] = backdrop_render($form['services'][$service]['svg']);

    switch ($version) {
      case '1.2.0':
        $row[] = backdrop_render($form['services'][$service]['fork']);
        break;
      case '5.x.x':
        $row[] = backdrop_render($form['services'][$service]['fav5']);
        break;
      case '4.7.0':
        $row[] = backdrop_render($form['services'][$service]['fav4']);
        break;
    }

    $row[] = backdrop_render($form['services'][$service]['legacy']); // Legacy

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
    $header = array(
      t('Icon'), 
      t('URL'), 
      t('SVG'),      
    );

    switch ($version) {
      case '1.2.0':
        $header[] = t('ForkAwesome');
        break;
      case '5.x.x':
        $header[] = t('FA v5');
        break;
      case '4.7.0':
        $header[] = t('FA v4');
        break;
    }

    $header[] = t('Legacy');
    $header[] = t('Weight');

    $variables = array(
      'header' => $header,
      'rows' => $rows, 
      'attributes' => array('id' => 'on-the-web-settings-table'),
    );
    $output .= theme('table', $variables);
  }

  // Pull out the submit button.
  $submit = backdrop_render($form['actions']);
  // Add the rest of the form elements on top.
  $output = backdrop_render_children($form) . $output . $submit;

  backdrop_add_tabledrag('on-the-web-settings-table', 'order', 'self', 'icon-weight');

  return $output;
}
