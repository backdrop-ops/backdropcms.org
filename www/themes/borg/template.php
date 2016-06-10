<?php
/**
 * @file
 * Theme function overrides.
 */

/*******************************************************************************
 * Alter functions: modify renderable structures before used.
 ******************************************************************************/

/**
 * Implements hook_form_id_alter()
 * Modify the user edit form for usability++
 */
function borg_form_user_profile_form_alter(&$form, &$form_state) {
  drupal_add_js('core/misc/vertical-tabs.js');
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


/*******************************************************************************
 * Preprocess functions: prepare variables for templates.
 ******************************************************************************/

/**
 * Prepares variables for page.tpl.php
 */
function borg_preprocess_page(&$variables) {
  // Add the Source Sans Pro font.
  backdrop_add_css('https://fonts.googleapis.com/css?family=Source+Sans+Pro:200,300,400,600,700', array('type' => 'external'));
  // Add FontAwesome.
  backdrop_add_js('https://use.fontawesome.com/baf3c35582.js', array('type' => 'external'));

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

  $node = menu_get_object();
  if (isset($node) && isset($node->type) && $node->type === 'book') {
    $variables['classes'][] = 'drawer-open';
    $variables['classes'][] = 'layout-both-sidebars';
  }
  else {
    $variables['classes'][] = 'drawer-closed';
    $array_key = array_search('layout-both-sidebars', $variables['classes']);
    if ($array_key !== FALSE) {
      $variables['classes'][$array_key] = 'layout-one-sidebar';
    }
  }
}

/**
 * Prepares variables for layout templates.
 */
function borg_preprocess_layout(&$variables) {
  if (arg(0) == 'user' && !is_numeric(arg(1))) {
    $variables['tabs'] = FALSE;
  }
}

/**
 * Preprocess views_view
 */
function borg_preprocess_views_view(&$variables) {
  if ($variables['name'] == 'modules') {
    $path = backdrop_get_path('theme', 'borg');
    backdrop_add_css($path . '/css/project-search.css');
  }
}

/**
 * Preprocess views exposed forms
 */
function borg_preprocess_views_exposed_form(&$variables) {
  if (substr($variables['form']['#id'], 0, 26) == 'views-exposed-form-modules'){
    // Update search field
    $search_field_key = '';
    $search_type = '';
    if (!empty($variables['form']['title'])){
      $search_field_key = 'title';
      if($variables['form']['#id'] == 'views-exposed-form-modules-page-2') {
        $search_type = 'themes';
      }
      elseif ($variables['form']['#id'] == 'views-exposed-form-modules-page-3') {
        $search_type = 'layouts';
      }
    }
    elseif (!empty($variables['form']['keys'])){
      $search_field_key = 'keys';
      $search_type = 'modules';
    }

    if (!empty($search_field_key)){
      // Boo divitis
      unset($variables['form'][$search_field_key]['#theme_wrappers']);
      // Add placeholder text
      $variables['form'][$search_field_key]['#attributes']['placeholder'] = t('Search '. $search_type .'...');
      // Re-render field
      $variables['widgets']['filter-'. $search_field_key]->widget = render($variables['form'][$search_field_key]);
    }
  }
}

/**
 * Prepare variables for node template
 */
function borg_preprocess_node(&$variables){
  // For news posts, change the username to a real name.
  if ($variables['node']->type == 'post') {
    // Change the submitted by language.
    $author = user_load($variables['node']->uid);
    $lang = $author->langcode;
    if (!empty($author->field_name[$lang])) {
      $variables['name'] = l($author->field_name[$lang][0]['safe_value'], 'user/' . $author->uid);
    }
  }

  // Change the submitted by language.
  $variables['submitted'] = t('Posted by !username on !datetime', array('!username' => $variables['name'], '!datetime' => $variables['date']));

  // Add a picture to blog posts.
  if ($variables['type'] == 'post' && $variables['view_mode'] == 'full') {
    // Get the profile photo.
    $author = user_load($variables['uid']);
    $langcode = $author->langcode;
    $variables['user_picture'] = theme('image_style', array('style_name' => 'medium', 'uri' => $author->field_photo[$langcode][0]['uri']));
  }
  if (substr($variables['type'], 0, 8) == 'project_'){
    $path = backdrop_get_path('theme', 'borg');
    $variables['content']['project_release_downloads']['#prefix'] = '<h2>' . t('Downloads')  . '</h2>';
    $variables['content']['project_release_downloads']['#weight'] = -10;
    backdrop_add_css($path . '/css/project-styles.css');
  }
}

/*******************************************************************************
 * Theme function overrides.
 ******************************************************************************/

function borg_form_element($variables) {
  $element = &$variables['element'];

  // This function is invoked as theme wrapper, but the rendered form element
  // may not necessarily have been processed by form_builder().
  $element += array(
    '#title_display' => 'before',
    '#wrapper_attributes' => array(),
  );
  $attributes = $element['#wrapper_attributes'];

  // Add element #id for #type 'item'.
  if (isset($element['#markup']) && !empty($element['#id'])) {
    $attributes['id'] = $element['#id'];
  }
  // Add element's #type and #name as class to aid with JS/CSS selectors.
  $attributes['class'][] = 'form-item';
  if (!empty($element['#type'])) {
    $attributes['class'][] = 'form-type-' . strtr($element['#type'], '_', '-');
  }
  if (!empty($element['#name'])) {
    $attributes['class'][] = 'form-item-' . strtr($element['#name'], array(' ' => '-', '_' => '-', '[' => '-', ']' => ''));
  }
  // Add a class for disabled elements to facilitate cross-browser styling.
  if (!empty($element['#attributes']['disabled'])) {
    $attributes['class'][] = 'form-disabled';
  }
  $output = '<div' . backdrop_attributes($attributes) . '>' . "\n";

  // If #title is not set, we don't display any label or required marker.
  if (!isset($element['#title'])) {
    $element['#title_display'] = 'none';
  }
  $prefix = isset($element['#field_prefix']) ? '<span class="field-prefix">' . $element['#field_prefix'] . '</span> ' : '';
  $suffix = isset($element['#field_suffix']) ? ' <span class="field-suffix">' . $element['#field_suffix'] . '</span>' : '';

  switch ($element['#title_display']) {
    case 'before':
    case 'invisible':
      if ($element['#type'] == 'textarea' || $element['#type'] == 'checkboxes' || $element['#type'] == 'radios' || 
         (array_key_exists('#field_name', $element) && $element['#field_name'] == 'field_expertise')) {
        $output .= ' ' . theme('form_element_label', $variables);
        if (!empty($element['#description'])) {
          $output .= '<div class="description">' . $element['#description'] . "</div>\n";
        }
        $output .= ' ' . $prefix . $element['#children'] . $suffix . "\n";
      }
      else {
        $output .= ' ' . theme('form_element_label', $variables);
        $output .= ' ' . $prefix . $element['#children'] . $suffix . "\n";
        if (!empty($element['#description'])) {
          $output .= '<div class="description">' . $element['#description'] . "</div>\n";
        }
      }
      break;

    case 'after':
      $output .= ' ' . $prefix . $element['#children'] . $suffix;
      $output .= ' ' . theme('form_element_label', $variables) . "\n";
      if (!empty($element['#description'])) {
        $output .= '<div class="description">' . $element['#description'] . "</div>\n";
      }
      break;

    case 'none':
    case 'attribute':
      if ($element['#type'] == 'password') {
        // Output no label and no required marker, only the children.
        if (!empty($element['#description'])) {
          $output .= '<div class="description">' . $element['#description'] . "</div>\n";
        }
        $output .= ' ' . $prefix . $element['#children'] . $suffix . "\n";
      }
      else {
        // Output no label and no required marker, only the children.
        $output .= ' ' . $prefix . $element['#children'] . $suffix . "\n";
        if (!empty($element['#description'])) {
          $output .= '<div class="description">' . $element['#description'] . "</div>\n";
        }
      }
      break;
  }

  $output .= "</div>\n";

  return $output;
}

/**
 * Custom theme output for widget.
 */
function borg_socialfield_drag_components($variables) {
  $element = $variables['element'];
  backdrop_add_tabledrag('socialfield-table', 'order', 'sibling', 'item-row-weight');
  $services = config_get('socialfield.settings', 'services');

  $header = array(t($element['#title']), '', '', '');
  $rows = array();
  $index = 0;

  for ($i=0; $i<$element['#num_elements']; $i++) {
    while (!isset($element['element_' . $index])) {
      // There is no element with this index. Moving on to the next possible element.
      $index++;
    }
    $current_element = $element['element_' . $index];

    $rows[] = array(
      'data' => array(
        '<div class="social-links">' .
          '<span class="socialfield socialfield-' . $current_element['#service'] . '">' .
            '<i class="icon ' . $services[$current_element['#service']]['icon'] . '">' . t($services[$current_element['#service']]['name']) . '</i>' .
          '</span>' .
        '</div>',
        backdrop_render($current_element['url']),
        backdrop_render($current_element['weight']),
        backdrop_render($current_element['operation']),
      ),
      'class' => array('draggable'),
      'weight' => $current_element['weight']['#value'],
    );

    $index++;
  }

  // Sorting elements by their weight.
  backdrop_sort($rows, array('weight'));

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array(
      'id' => 'socialfield-table',
    ),
  ));
  $output .= '<div class="description">' . backdrop_render($element['description']) . '</div>';
  $output .= backdrop_render($element['add_one_social']);

  return $output;
}

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
    // Add the font awesome icon where there's a drawer.
    if ($element['#href'] && $element['#href'] == 'node/1') {
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
    return '<i class="fa fa-twitter-square"></i><span class="element-invisible">Backdrop CMS on Twitter</span>';
  }
  if ($variables['service'] == 'facebook') {
    return '<i class="fa fa-facebook-square"></i><span class="element-invisible">Backdrop CMS on Facebook</span>';
  }
  if ($variables['service'] == 'google') {
    return '<i class="fa fa-google-plus-square"></i><span class="element-invisible">Backdrop CMS on Google Plus</span>';
  }
  if ($variables['service'] == 'youtube') {
    return '<i class="fa fa-youtube-square"></i><span class="element-invisible">Backdrop CMS on YouTube</span>';
  }
  if ($variables['service'] == 'rss') {
    return '<i class="fa fa-rss-square"></i><span class="element-invisible">Latest News from Backdrop CMS</span>';
  }
}

/**
 * Overrides theme_feed_icon().
 */
function borg_feed_icon($variables) {
  $text = t('Subscribe to !feed-title', array('!feed-title' => $variables['title']));
  $image = '<i class="fa fa-rss-square"></i><span class="element-invisible">' . $text . '</span>';
  return l($image, $variables['url'], array('html' => TRUE, 'attributes' => array('class' => array('feed-icon'), 'title' => $text)));
}
