<?php

/**
 * Helper function to add CSS from this theme with the options we want
 */
function _basis_borg_add_css($data, $options = array()) {
  $basis_borg_path = backdrop_get_path('theme', 'basis_borg');
  $default_options = array(
    'group' => CSS_THEME,
    'weight' => 900
  );

  $options = array_merge($default_options, $options);
  if (!isset($options['type'])) {
    backdrop_add_css($basis_borg_path . '/css/' . $data, $options);
  }
  else {
    backdrop_add_css($data, $options);
  }
}

/**
 * Implements template_preprocess_layout().
 */
function basis_borg_preprocess_layout(&$variables) {
  $basis_borg_path = backdrop_get_path('theme', 'basis_borg');

  // Add homepage stylesheet to homepage
  if ($variables['is_front']) {
    _basis_borg_add_css('layout/front.css');
  }

  // Add helpful CSS class to layout based on layout name
  $variables['classes'][] = backdrop_clean_css_identifier('layout--name-' . $variables['layout']->name);

  switch ($variables['layout']->name) {
    case 'search_modules':
    case 'search_themes':
    case 'search_layouts':
      $variables['classes'][] = 'layout--category-project-search';
      _basis_borg_add_css('layout/project-search.css');
      break;
    case 'projects':
      _basis_borg_add_css('layout/project.css');
      $variables['classes'][] = 'layout--category-project';
      break;
    case 'services':
      _basis_borg_add_css('layout/services.css');
      break;
  }

  if (arg(0) == 'user' && !is_numeric(arg(1))) {
    $variables['tabs'] = FALSE;
  }

  // Process nodes
  if (arg(0) == 'node' && is_numeric(arg(1)) && !arg(2)) {
    _basis_borg_preprocess_layout__node($variables);
  }
  // Add processing for user layouts
  else if (arg(0) == 'user' && is_numeric(arg(1))) {
    _basis_borg_preprocess_layout__user($variables);
  }
}

/**
 * Implements template_preprocess_node().
 */
function basis_borg_preprocess_node(&$variables) {
  if (!empty($variables['node']->uid)) {
    $author = user_load($variables['node']->uid);
    if (!empty($author->field_photo)) {
      $variables['user_picture_render_array'] = field_view_field('user', $author, 'field_photo');
      $variables['user_picture_render_array']['#label_display'] = 'hidden';
      $variables['user_picture_render_array'][0]['#image_style'] = 'headshot_small';
      $variables['user_picture'] = render($variables['user_picture_render_array']);
    }
  }
  if (isset($author->field_name[LANGUAGE_NONE][0]['safe_value'])) {
    $username = $author->field_name[LANGUAGE_NONE][0]['safe_value'];
  }
  else {
    $username = $variables['name'];
  }

  $variables['submitted'] = t('By !username on !datetime', array('!username' => $username, '!datetime' => $variables['date']));

  // Get rid of redundant heading tag
  unset($variables['content']['project_release_downloads']['#prefix']);

  // Preprocess layouts by node type
  $function = '_' . __FUNCTION__ . '__' . $variables['node']->type;
  if (function_exists($function)) {
    $function($variables);
  }
}

/**
 * Implements template_preprocess_field().
 */
function basis_borg_preprocess_field(&$variables) {
}

/**
 * Implements template_preprocess_views_view().
 */
function basis_borg_preprocess_views_view(&$variables) {
  $basis_borg_path = backdrop_get_path('theme', 'basis_borg');
  $css_options = array(
    'group' => CSS_THEME,
    'weight' => 900
  );

  switch ($variables['view']->name) {
    case 'news':
      _basis_borg_add_css('component/news-listing.css');
      break;
    case 'events':
      _basis_borg_add_css('component/events-listing.css');
  }

}

/**
 * Overrides theme_menu_tree().
 */
function basis_borg_menu_tree(&$variables) {
  if ($variables['theme_hook_original'] == 'menu_tree__main_menu' && $variables['depth'] == 0 && isset($variables['attributes']['data-menu-style']) && $variables['attributes']['data-menu-style'] == 'dropdown') {
    if (empty($variables['attributes']['id'])) {
      $variables['attributes']['id'] = 'header-main-menu';
    }
    $burger_toggler = '<input id="burger-toggler--state" class="burger-toggler--state element-invisible" type="checkbox" aria-controls="' . $variables['attributes']['id'] .'">'.
      '<label class="burger-toggler__button" for="burger-toggler--state">' .
        '<span class="burger-toggler__button-icon"></span><span class="burger-toggler__button-text">Menu</span>' .
        '<span class="burger-toggler__assistive-text element-invisible">Toggle main menu visibility</span>'.
      '</label>';

    return $burger_toggler . '<ul ' . backdrop_attributes($variables['attributes']) . '>' . $variables['tree'] . '</ul>';
  }
  return '<ul ' . backdrop_attributes($variables['attributes']) . '>' . $variables['tree'] . '</ul>';
}

/**
 * Implements hook_form_id_alter()
 * Modify the user edit form for usability++
 */
function basis_borg_form_user_profile_form_alter(&$form, &$form_state) {
  backdrop_add_js('core/misc/vertical-tabs.js');
  $account_fieldset = array(
    '#type'         => 'fieldset',
    '#title'        => t('Change Email or Password'),
    '#collapsible'  => true,
    '#collapsed'    => true,
    '#weight'       => -9,
  );

  $fields_for_account_fieldset = array('current_pass', 'mail', 'pass');
  foreach ($fields_for_account_fieldset as $field_name) {
    if (isset($form['account'][$field_name])) {
      $account_fieldset[$field_name] = $form['account'][$field_name];
      hide($form['account'][$field_name]);
    }
  }
  $form['account']['account_fieldset'] = $account_fieldset;

  $form['account']['#weight'] = 1;
  $form['account']['name']['#weight'] = -50;
  $form['field_name']['#weight'] = -51;

  $form['field_forhire']['#weight'] = 2;
  $form['field_services']['#weight'] = 3;
  $form['field_expertise']['#weight'] = 4;

  $form['field_bio']['#weight'] = 5;
  $form['field_photo']['#weight'] = 6;
  $form['field_header_photo']['#weight'] = 7;
  $form['field_gender']['#weight'] = 8;
  $form['field_gender'][LANGUAGE_NONE]['#options']['_none'] = t('- Not specified -');
  $form['field_industries']['#weight'] = 9;

  $social_fieldset = array(
    '#type'         => 'fieldset',
    '#title'        => t('Find me Online'),
    '#collapsible'  => true,
    '#collapsed'    => false,
    '#weight'       => 10,
  );

  $form['field_social']['#weight'] = 1;
  $form['field_irc']['#weight'] = 2;
  $form['field_websites']['#weight'] = 3;

  $fields_for_account_fieldset = array('field_irc', 'field_social', 'field_websites');
  foreach ($fields_for_account_fieldset as $field_name) {
    $social_fieldset[$field_name] = $form[$field_name];
    hide($form[$field_name]);
  }
  $form['social_fieldset'] = $social_fieldset;

  $form['field_contributions']['#weight'] = 11;
  $form['field_contributions_other']['#weight'] = 12;

  $form['contact']['#weight'] = 21;
  $form['timezone']['#weight'] = 22;
  $form['timezone']['#collapsed'] = TRUE;
  $form['redirect']['#weight'] = 23;
}

/**
 * Helper function for node layouts
 */
function _basis_borg_preprocess_layout__node(&$variables) {
  $node = node_load(arg(1));

  // Preprocess layouts by node type
  $function = __FUNCTION__ . '_' . $node->type;
  if (function_exists($function)) {
    $function($variables, $node);
  }
}

/**
 * Helper function for showcase node layouts
 */
function _basis_borg_preprocess_layout__node_showcase(&$variables, $node) {
  // Special handling for header image.
  $lang = $node->langcode;
  $variables['top_attributes']['class'][] = 'showcase';

  $basis_borg_path = backdrop_get_path('theme', 'basis_borg');
  $css_options = array(
    'group' => CSS_THEME,
    'weight' => 900
  );
  _basis_borg_add_css('layout/showcase.css');

  // Check to see if there is a hero image.
  if (isset($node->field_header_photo[$lang][0]['uri'])) {
    // Generate an image at the correct size.
    $image = image_style_url('header', $node->field_header_photo[$lang][0]['uri']);
    $top_bg_image = '.l-top { background-image: url(' . $image . ')}';

    _basis_borg_add_css($top_bg_image, array(
      'type' => 'inline',
      'weight' => 800
    ));

    // Add an addidional class.
    $variables['classes'][] = 'layout--has-top-background';
  }
}

/**
 * Helper function for user layouts
 */
function _basis_borg_preprocess_layout__user(&$variables) {
  _basis_borg_add_css('layout/account.css');

  $variables['classes'][] = 'account-page';
  if (arg(2)) {
    $variables['classes'][] = 'account-page--edit';
  }
  else {
    // @todo I needed to set theme_hook_suggestion and the array theme_hook_suggestions.. ??
    $variables['theme_hook_suggestion'] = 'layout__moscone__account';
    $variables['theme_hook_suggestions'] = array('layout__moscone__account');

    // Special handling for header image.
    // Check to see if there is a profile image.
    $account = user_load(arg(1)); // Entity cache should save us here?
    if (isset($account->field_header_photo[LANGUAGE_NONE][0]['uri'])) {
      // Generate an image at the correct size.
      $image = image_style_url('header', $account->field_header_photo[LANGUAGE_NONE][0]['uri']);
      $top_bg_image = '.l-wrapper:before {background-image: url(' . $image . ');}';
      _basis_borg_add_css($top_bg_image, array(
        'type' => 'inline',
        'weight' => 800
      ));

      // Add an additional class.
      $variables['classes'][] = 'layout--has-wrapper-background';
    }
  }
}

/**
 * Helper function for showcase nodes
 */
function _basis_borg_preprocess_node__showcase(&$variables) {
  $node = $variables['node'];
  $lang = $node->langcode;

  _basis_borg_add_css('component/screenshot.css');

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
  $output = '';
  if ($desktop_count) {
    foreach ($node->field_screen_lg[$lang] as $delta => $info) {
      $image = theme('image_style', array('style_name' => 'large', 'uri' => $node->field_screen_lg[$lang][$delta]['uri']));
      $output .= '<div class="row showcase-highlight-row">';
      $output .= '  <div class="col-xs-12">';
      $output .= '    <div class="browser-ui">';
      $output .= '      <div class="frame">';
      $output .= '        <span class="red"></span>';
      $output .= '        <span class="yellow"></span>';
      $output .= '        <span class="green"></span>';
      $output .= '      </div>';
      $output .= '      ' . $image;
      $output .= '    </div>';
      $output .= '  </div>';
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
        $combo_rows[$delta] = '<div class="row showcase-highlight-row">' . $output . '</div>';
      }
    }
  }

  $quote_rows = array();
  if (!empty($node->field_pullquote[$lang])) {
    $output = '';
    foreach ($node->field_pullquote[$lang] as $delta => $info) {
      $output .= '<div class="row showcase-highlight-row">';
      $output .=   '<div class="col-xs-12">';
      $output .=     '<blockquote>';
      $output .=       check_markup($info['value'], $info['format']);
      $output .=     '</blockquote>';
      $output .=   '</div>';
      $output .= '</div>';
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

  $variables['rows_output'] = '';
  foreach ($rows as $var_index) {
    // First check for a desktop screenshot.
    if (!empty($desktop_rows) && ($last != 'desktop')) {
      $variables['rows_output'] .= array_shift($desktop_rows);
      $last = 'desktop';
      continue;
    }
    // Check for quotes.
    elseif (!empty($quote_rows) && ($last != 'quote')) {
      $variables['rows_output'] .= array_shift($quote_rows);
      $last = 'quote';
      continue;
    }
    // Check for other screenshots.
    elseif (!empty($combo_rows)) {
      $variables['rows_output'] .= array_shift($combo_rows);
      $last = 'combo';
      continue;
    }
  }
}
