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

  if (arg(0) == 'modules' || arg(0) == 'themes' || arg(0) == 'layouts') {
    $variables['classes'][] = 'project-search';

    $path = backdrop_get_path('theme', 'borg');
    backdrop_add_css($path . '/css/project-search.css');
  }
  elseif (arg(0) == 'showcase') {
    $variables['classes'][] = 'showcase';
  }

  if (module_exists('admin_bar') && user_access('admin_bar')) {
    $variables['classes'][] = 'admin-bar';
  }

  if (arg(0) == 'user') {
    if (arg(1) == 'login') {
      $variables['classes'][] = 'user-form';
      $variables['classes'][] = 'user-login';
    }
    if (arg(1) == 'register') {
      $variables['classes'][] = 'user-form';
      $variables['classes'][] = 'user-reister';
    }
    elseif (arg(1) == 'password') {
      $variables['classes'][] = 'user-form';
      $variables['classes'][] = 'user-password';
    }
    elseif (is_numeric(arg(1)) && ! arg(2)) {
      $variables['classes'][] = 'account-page';
    }
    else {
      global $user;
      if ($user->uid == 0 && !arg(1)) {
        $variables['classes'][] = 'user-form';
        $variables['classes'][] = 'user-login';
      }
    }
  }

  // Add CSS for the try-out-backdrop-cms page.
  if (request_path() == 'try-out-backdrop-cms') {
    backdrop_add_css(
      backdrop_get_path('theme', 'borg') . '/css/try-out-backdropcms.css'
    );
  }
}

/**
 * Prepares variables for layout templates.
 */
function borg_preprocess_layout(&$variables) {
  $variables['wrap_attributes'] = array('class' => array('l-wrapper'));

  if (arg(0) == 'user' && !is_numeric(arg(1))) {
    $variables['tabs'] = FALSE;
  }
  // Special handling for header image.
  if (arg(0) == 'user' && is_numeric(arg(1)) && !arg(2)) {
    // Check to see if there is a profile image.
    $account = user_load(arg(1)); // Entity cache should save us here?
    if (isset($account->field_header_photo[LANGUAGE_NONE][0]['uri'])) {
      // Generate an image at the correct size.
      $image = image_style_url('header', $account->field_header_photo[LANGUAGE_NONE][0]['uri']);
      $variables['wrap_attributes']['style'] = 'background-image: url(' . $image . ')';
      // Add an addidional class.
      $variables['wrap_attributes']['class'][] = 'has-background';
    }
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
  // Change the submitted by language for all nodes.
  $variables['submitted'] = t('Posted by !username on !datetime', array('!username' => $variables['name'], '!datetime' => $variables['date']));

  // For news posts, change the username to a real name.
  if ($variables['node']->type == 'post') {
    // Change the submitted by language.
    $author = user_load($variables['node']->uid);
    $lang = $author->langcode;
    if (!empty($author->field_name[$lang])) {
      $variables['name'] = l($author->field_name[$lang][0]['safe_value'], 'user/' . $author->uid);
    }
  }

  // For blog posts, add a picture.
  if ($variables['type'] == 'post' && $variables['view_mode'] == 'full') {
    // Get the profile photo.
    $author = user_load($variables['uid']);
    $langcode = $author->langcode;
    $variables['user_picture'] = theme('image_style', array('style_name' => 'medium', 'uri' => $author->field_photo[$langcode][0]['uri']));
  }

  // For project nodes...
  if (($variables['type'] == 'core') || substr($variables['type'], 0, 8) == 'project_'){
    $path = backdrop_get_path('theme', 'borg');
    unset($variables['content']['project_release_downloads']['#prefix']);
    $variables['classes'][] = 'node-project';
    backdrop_add_css($path . '/css/project-styles.css');
  }
}

/**
 * Prepares variables for views-view-grid.tpl.php
 */
function borg_preprocess_views_view_grid(&$variables) {
  $view     = $variables['view'];
  $cols     = $view->style_plugin->options['columns'];
  $rows     = $variables['rows'];

  // These views have the columns stay wider at smaller screensizes.
  $sm_grid_views = array('todo');

  if (in_array($view->name, $sm_grid_views)) {
    $column_classes = array(
      1 => 'col-sm-12',
      2 => 'col-sm-6',
      3 => 'col-sm-4',
      4 => 'col-sm-3',
      5 => 'col-sm-5ths',
      6 => 'col-sm-2',
    );
  }
  else {
    $column_classes = array(
      1 => 'col-md-12',
      2 => 'col-md-6',
      3 => 'col-md-4',
      4 => 'col-md-3',
      5 => 'col-md-5ths',
      6 => 'col-md-2',
    );
  }

  $col_class = $column_classes[$cols];

  // Apply the radix classes
  foreach ($rows as $row_number => $row) {
    $variables['row_classes'][$row_number][] = 'row';
    $variables['row_classes'][$row_number][] = 'row-fluid';
    foreach ($rows[$row_number] as $column_number => $item) {
      $variables['column_classes'][$row_number][$column_number][] = $col_class;
    }
  }
  $variables['classes'][] = 'container-fluid';
}

/**
 * Processes variables for book-navigation.tpl.php.
 *
 * @param $variables
 *   An associative array containing the following key:
 *   - book_link
 *
 * @see book-navigation.tpl.php
 */
function borg_preprocess_book_navigation(&$variables) {
  $book_link = $variables['book_link'];

  if ($book_link['mlid']) {
    // Change the previous link.
    if ($prev = borg_book_prev($book_link)) {
      $prev_href = url($prev['href']);
      backdrop_add_html_head_link(array('rel' => 'prev', 'href' => $prev_href));
      $variables['prev_url'] = $prev_href;
      $variables['prev_title'] = check_plain($prev['title']);
    }
    // Change the next link.
    if ($next = borg_book_next($book_link)) {
      $next_href = url($next['href']);
      backdrop_add_html_head_link(array('rel' => 'next', 'href' => $next_href));
      $variables['next_url'] = $next_href;
      $variables['next_title'] = check_plain($next['title']);
    }
  }

  // Re-check the has links status since it was altered above.
  $variables['has_links'] = FALSE;
  // Link variables to filter for values and set state of the flag variable.
  $links = array('prev_url', 'prev_title', 'parent_url', 'parent_title', 'next_url', 'next_title');
  foreach ($links as $link) {
    if (isset($variables[$link])) {
      // Flag when there is a value.
      $variables['has_links'] = TRUE;
    }
    else {
      // Set empty to prevent notices.
      $variables[$link] = '';
    }
  }
}

/**
 * Gets the previous Book link - ignoring child pages.
 *
 * @param  $book_link
 *   A fully loaded menu link that is part of the book hierarchy.
 *
 * @return
 *   A fully loaded menu link for the page before the one represented in $book_link.
 */
function borg_book_prev($book_link) {
  // If the parent is zero, we are at the start of a book.
  if ($book_link['plid'] == 0) {
    return NULL;
  }
  $flat = book_get_flat_menu($book_link);
  // Remove child pages from next/prev links.
  foreach ($flat as $key => $item) {
    if ($item['depth'] > 2) {
      unset($flat[$key]);
    }
  }

  // Assigning the array to $flat resets the array pointer for use with each().
  $curr = NULL;
  do {
    $prev = $curr;
    list($key, $curr) = each($flat);
  } while ($key && $key != $book_link['mlid']);

  return $prev;
}

/**
 * Gets the next Book link - ignoring child pages.
 *
 * @param  $book_link
 *   A fully loaded menu link that is part of the book hierarchy.
 *
 * @return
 *   A fully loaded menu link for the page after the one represented in $book_link.
 */
function borg_book_next($book_link) {
  $flat = book_get_flat_menu($book_link);
  // Remove child pages from next/prev links.
  foreach ($flat as $key => $item) {
    if ($item['depth'] > 2) {
      unset($flat[$key]);
    }
  }

  // Assigning the array to $flat resets the array pointer for use with each().
  do {
    list($key, $curr) = each($flat);
  }
   while ($key && $key != $book_link['mlid']);

  if ($key == $book_link['mlid']) {
    return current($flat);
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

function borg_menu_local_tasks($variables) {
  $output = '';

  if (!empty($variables['primary'])) {
    // Remove the releases tab.
    if (arg(0) == 'node' && is_numeric(arg(1)) && !arg(2)) {
      foreach ($variables['primary'] as $key => $link) {
        if (strstr($link['#link']['path'], '/releases')) {
          unset($variables['primary'][$key]);
        }
      }
    }

    if (count($variables['primary']) > 1) {
      $variables['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
      $variables['primary']['#prefix'] .= '<ul class="tabs primary">';
      $variables['primary']['#suffix'] = '</ul>';
      $output .= backdrop_render($variables['primary']);
    }
  }
  if (!empty($variables['secondary'])) {
    $variables['secondary']['#prefix'] = '<h2 class="element-invisible">' . t('Secondary tabs') . '</h2>';
    $variables['secondary']['#prefix'] .= '<ul class="tabs secondary">';
    $variables['secondary']['#suffix'] = '</ul>';
    $output .= backdrop_render($variables['secondary']);
  }

  return $output;
}

/**
 * Overrides theme_github_info().
 */
function borg_github_info($variables) {
  $url = 'https://github.com/' . $variables['github_path'];
  $clone_url = $url . '.git';

  if ($variables['github_path'] != 'backdrop/backdrop') {
    $items = array(
      l(t('Project page'), $url),
      l(t('Issue Queue'), $url . '/issues'),
      l(t('Documentation'), $url . '/wiki'),
    );
  }
  else {
    $items = array(
      l(t('Project page'), $url),
      l(t('Issue Queue'), $url . '-issues/issues'),
      l(t('Documentation'), 'user-guide'),
    );
  }


  $list = theme('item_list', array('items' => $items, 'title' => t('GitHub')));

  $clone  = '<div class="github-clone">';
  $clone .= '<label class="github-clone-label">' . t('Clone URL') . '</label>';
  $clone .= '<input type="text" readonly="" aria-label="Clone this repository at ' . $clone_url . '" value="' . $clone_url . '">';
  $clone .= '</div>';

  return $list . $clone;
}
