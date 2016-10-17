<?php
/**
 * @file
 * PHP functions for the drawer layout.
 */

/**
 * Prepare variables for the drawer layout template file.
 */
function template_preprocess_layout__double_fixed_case(&$variables) {
  // Define all the template variables.
  $variables['row_first'] = FALSE;
  $variables['row_second'] = FALSE;
  $variables['row_third'] = FALSE;
  $variables['row_fourth'] = FALSE;
  $variables['row_fifth'] = FALSE;
  $variables['row_sixth'] = FALSE;

  if ($variables['content']['sidebar'] && $variables['content']['drawer']) {
    $variables['classes'][] = 'layout-both-sidebars';
  }
  elseif ($variables['content']['sidebar'] || $variables['content']['drawer']) {
    $variables['classes'][] = 'layout-one-sidebar';

    if ($variables['content']['sidebar']) {
      $variables['classes'][] = 'layout-has-sidebar';
    }
    else {
      $variables['classes'][] = 'layout-has-drawer';
    }
  }
  else {
    $variables['classes'][] = 'layout-no-sidebars';
  }

  // Add region class via backdrop_attributes().
  $variables['top_attributes'] = array('class' => array('l-content-top'));

  // Special handling for header image.
  if (arg(0) == 'node' && is_numeric(arg(1)) && !arg(2)) {
    $node = node_load(arg(1));
    $lang = $node->langcode;
    if ($node->type == 'showcase') {
      $variables['top_attributes']['class'][] = 'showcase';

      // Check to see if there is a hero image.
      if (isset($node->field_header_photo[$lang][0]['uri'])) {
        // Generate an image at the correct size.
        $image = image_style_url('header', $node->field_header_photo[$lang][0]['uri']);
        $variables['top_attributes']['style'] = 'background-image: url(' . $image . ')';
        // Add an addidional class.
        $variables['top_attributes']['class'][] = 'has-background';
      }

      // Check counts of each type of photos.
      $desktop_count = $tablet_count = $phone_count = 0;
      if (!empty($node->field_screen_lg)) {
        $desktop_count = count($node->field_screen_lg[$lang]);
      }
      if (!empty($node->field_screen_md)) {
        $tablet_count = count($node->field_screen_md[$lang]);
      }
      if (!empty($node->field_screen_sm)) {
        $phone_count = count($node->field_screen_sm[$lang]);
      }

      // Assemble the desktop photos into individual rows.
      $desktop_rows = array();
      if ($desktop_count) {
        foreach ($node->field_screen_lg[$lang] as $delta => $info) {
          $image = theme('image_style', array('style_name' => 'large', 'uri' => $node->field_screen_lg[$lang][$delta]['uri']));
          $output  = '<div class="browser-ui">';
          $output .= '  <div class="frame">';
          $output .= '    <span class="red"></span>';
          $output .= '    <span class="yellow"></span>';
          $output .= '    <span class="green"></span>';
          $output .= '  </div>';
          $output .= '  ' . $image;
          $output .= '</div>';
          $desktop_rows[$delta] = $output;
        }
      }

      $combo_rows = array();
      if ($tablet_count && $phone_count) {
        foreach ($node->field_screen_md[$lang] as $delta => $info) {
          $tablet = theme('image_style', array('style_name' => 'tablet', 'uri' => $node->field_screen_md[$lang][$delta]['uri']));
          if (isset($node->field_screen_sm[$lang][$delta])) {
            $phone = theme('image_style', array('style_name' => 'phone', 'uri' => $node->field_screen_sm[$lang][$delta]['uri']));
            $output  = '';
            if ($delta/2) {
              $output .= '<div class="col-sm-4">';
              $output .= '<div class="screen">';
              $output .= '  <div class="phone-ui">';
              $output .= '    <span class="bar"></span>';
              $output .= $phone;
              $output .= '    <span class="dot"></span>';
              $output .= '  </div>';
              $output .= '</div>';
              $output .= '</div>';
            }
            $output .= '<div class="col-sm-8">';
            $output .= '<div class="screen">';
            $output .= '  <div class="tablet-ui">';
            $output .= '    <span class="camera"></span>';
            $output .= $tablet;
            $output .= '    <span class="dot"></span>';
            $output .= '  </div>';
            $output .= '</div>';
            $output .= '</div>';
            if (!$delta/2) {
              $output .= '<div class="col-sm-4">';
              $output .= '<div class="screen">';
              $output .= '  <div class="phone-ui">';
              $output .= '    <span class="bar"></span>';
              $output .= $phone;
              $output .= '    <span class="dot"></span>';
              $output .= '  </div>';
              $output .= '</div>';
              $output .= '</div>';
            }
            $combo_rows[$delta] = $output;
          }
        }
      }

      $quote_rows = array();
      if (!empty($node->field_pullquote[$lang])) {
        foreach ($node->field_pullquote[$lang] as $delta => $info) {
          $output  = '<blockquote>';
          $output .= check_markup($info['value'], $info['format']);
          $output .= '</blockquote>';
          $quote_rows[$delta] = $output;
        }
      }

      // Assemble the rows.
      $rows = array(
        'row_first',
        'row_second',
        'row_third',
        'row_fourth',
        'row_fifth',
        'row_sixth',
      );

      $last = 'none';
      foreach ($rows as $var_index) {
        // First check for a desktop screenshot.
        if (!empty($desktop_rows) && ($last != 'desktop')) {
          $variables[$var_index] = array_shift($desktop_rows);
          $last = 'desktop';
          continue;
        }
        // Check for quotes.
        elseif (!empty($quote_rows) && ($last != 'quote')) {
          $variables[$var_index] = array_shift($quote_rows);
          $last = 'quote';
          continue;
        }
        // Check for other screenshots.
        elseif (!empty($combo_rows)) {
          $variables[$var_index] = array_shift($combo_rows);
          $last = 'combo';
          continue;
        }
      }

    }
  }
}
