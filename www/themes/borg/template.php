<?php
/**
 * @file
 * Theme function overrides.
 */

/*******************************************************************************
 * New theme functions.
 ******************************************************************************/

/**
 * Implements hook_theme()
 * - Provides a theme function for outputting a list.
 */
function borg_theme($existing, $type, $theme, $path) {
  return array(
    'borg_list' => array(
      'variables' => array(
        'type' => 'ul',
        'items' => array(),
        'attributes' => array(),
        'empty' => NULL,
      ),
    ),
  );
}

/**
 * Outputs a HTML list.
 * - type: UL or OL
 * - items: A list of items to render. String values are rendered as is. Each item can also be an associative array containing:
 *   - data: The string content of the list item.
 *   - attrubutes: Any attributes to be applied to the LI list item.
 *   - children: A list of nested child items to render that behave identically to 'items', but any non-numeric string keys are treated as HTML attributes for the child list that wraps 'children'.
 */
function borg_borg_list($variables) {
  $type = $variables['type'];
  $items = $variables['items'];
  $list_attributes = $variables['attributes'];

  $output = '';

  if ($items) {
    $output .= '<' . $type . backdrop_attributes($list_attributes) . '>';
    $num_items = count($items);
    $i = 0;

    foreach ($items as $key => $item) {
      $i++;
      $attributes = array();

      if (is_array($item)) {
        $value = '';
        if (isset($item['data'])) {
          $value .= $item['data'];
        }

        if (isset($item['attributes'])) {
          $attributes = $item['attributes'];
        }

        // Append nested child list, if any.
        if (isset($item['children'])) {
          // Handle child attributes.
          $child_list_attributes = array();
          foreach ($item['children'] as $child_key => $child_item) {
            if (is_string($child_key)) {
              $child_list_attributes = $child_item['attributes'];
              unset($item['children'][$child_key]);
            }
          }
          $value .= theme('borg_list', array(
            'items' => $item['children'],
            'type' => $type,
            'attributes' => $child_list_attributes,
          ));
        }
      }
      else {
        $value = $item;
      }

      $output .= '<li' . backdrop_attributes($attributes) . '>' . $value . '</li>';
    }
    $output .= "</$type>";
  }
  elseif (!empty($variables['empty'])) {
    $output .= render($variables['empty']);
  }

  return $output;
}


/*******************************************************************************
 * Alter functions: modify renderable structures before used.
 ******************************************************************************/

/**
 * Implements hook_form_FORM_ID_alter().
 */
function borg_form_user_register_form_alter(&$form, &$form_state) {
  $help = t('Already have an account?') . ' ' . l(t('Log in instead'), 'user/login') . '.';
  $form['login'] = array(
    '#type' => 'help',
    '#markup' => $help,
    '#weight' => -100,
  );

  // Remove description text from password.
  unset($form['account']['pass']['#description']);
  // Fix the description text for email address.
  $form['account']['mail']['#description'] = t('This e-mail address is not made public and will only be used if you choose to receive messages.');
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function borg_form_user_login_alter(&$form, &$form_state) {
  $help = t('Don\'t have an account yet?') . ' ' . l(t('Create one now'), 'user/register') . '.';
  $form['login'] = array(
    '#type' => 'help',
    '#markup' => $help,
    '#weight' => -100,
  );

  // Add a forgot password link, since tabs are removed.
  $form['actions']['forgot'] = array(
    '#markup' => '<small>' . l(t('Forgot password?'), 'user/password') . '</small>',
    '#weight' => 10,
  );
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Removes useless description text from log in form, adds additional links.
 */
function borg_form_user_pass_alter(&$form, &$form_state) {
  // Add a log in link, since tabs are removed.
  $form['actions']['forgot'] = array(
    '#markup' => '<small>' . l(t('Log in'), 'user/login') . '</small>',
    '#weight' => 10,
  );
}

/**
 * Implements hook_menu_alter().
 *
 * Removes tabs from the log in and password pages.
 */
function borg_menu_alter(&$items) {
  $items['user/login']['type'] = MENU_CALLBACK;
  $items['user/pasword']['type'] = MENU_CALLBACK;
  $items['node']['page callback'] = 'backdrop_not_found';
}

/*******************************************************************************
 * Preprocess functions: prepare variables for templates.
 ******************************************************************************/

/**
 * Prepares variables for page templates.
 * @see page.tpl.php
 */
function borg_preprocess_page(&$variables) {
  $arg0 = check_plain(arg(0));
  $arg1 = check_plain(arg(1));
  $arg2 = check_plain(arg(2));

  // Add the Source Sans Pro font.
  $source_sans = 'https://fonts.googleapis.com/css?family=Source+Sans+Pro:200,300,400,600,700';
  backdrop_add_css($source_sans, array('type' => 'external'));

  // Make the icons available for use in CSS.
  $icons_needed = array('user-circle');
  backdrop_add_icons($icons_needed);

  // Add FontAwesome.
  //$font_awesome = 'https://use.fontawesome.com/baf3c35582.js';
  //backdrop_add_js($font_awesome, array('type' => 'external'));

  // Add ForkAwesome. @todo - replace with ICON API
  $fork_awesome = 'https://cdn.jsdelivr.net/npm/fork-awesome@1.2.0/css/fork-awesome.min.css';
  $attributes = array(
    'integrity' => 'sha256-XoaMnoYC5TH6/+ihMEnospgm0J1PM/nioxbOUdnM8HY=',
    'crossorigin' => 'anonymous',
  );
  backdrop_add_css($fork_awesome, array('type' => 'external', 'attributes' => $attributes));

  // Load IBM Plex variable fonts.
  backdrop_add_css(
    'https://fonts.googleapis.com/css2?family=IBM+Plex+Mono:ital,wght@0,100;0,200;0,300;0,400;0,500;0,600;0,700;1,100;1,200;1,300;1,400;1,500;1,600;1,700&family=IBM+Plex+Sans:ital,wght@0,100..700;1,100..700&display=swap',
    array('type' => 'external')
  );

  // Add a body class based on the admin bar.
  if (module_exists('admin_bar') && user_access('admin_bar')) {
    $variables['classes'][] = 'admin-bar';
  }

  $path = backdrop_get_path('theme', 'borg');
  // Add Flexslider to the front page only.
  if (backdrop_is_front_page()) {
    backdrop_add_css($path . '/css/page-front.css');
  }
  elseif ($arg0 == 'support') {
    if ($arg1 == 'services') {
      backdrop_add_css($path . '/css/page-services.css');
    }
  }
  elseif ($arg0 == 'modules' || $arg0 == 'themes' || $arg0 == 'layouts') {
    $variables['classes'][] = 'project-search';
    backdrop_add_css($path . '/css/page-project-search.css');
  }
  elseif ($arg0 == 'user') {
    if ($arg1 == 'login') {
      $variables['classes'][] = 'user-form';
      $variables['classes'][] = 'user-login';
    }
    if ($arg1 == 'register') {
      $variables['classes'][] = 'user-form';
      $variables['classes'][] = 'user-reister';
    }
    elseif ($arg1 == 'password') {
      $variables['classes'][] = 'user-form';
      $variables['classes'][] = 'user-password';
    }
    else {
      global $user;
      if ($user->uid == 0 && !$arg1) {
        $variables['classes'][] = 'user-form';
        $variables['classes'][] = 'user-login';
      }
      elseif (is_numeric($arg1) && !$arg2 || ($user->uid)) {
        $variables['classes'][] = 'profile-page';
        backdrop_add_css($path . '/css/page-profile.css');
      }
    }
  }

  // Add a class based on the node ID...
  if ($arg0 == 'node' && is_numeric($arg1) && !$arg2) {
    $variables['classes'][] = 'node-' . $arg1;
  }

  // ...or add classes based on args.
  elseif ($arg0) {
    $variables['classes'][] = $arg0;
    if ($arg1) {
      $variables['classes'][] = $arg0 . '-' . $arg1;
      if ($arg2) {
        $variables['classes'][] = $arg0 . '-' . $arg1 . '-' . $arg2;
      }
    }
  }
}

/**
 * Prepares variables for layout templates.
 * @see layout.tpl.php
 */
function borg_preprocess_layout(&$variables) {
  $arg0 = check_plain(arg(0));
  $arg1 = check_plain(arg(1));
  $arg2 = check_plain(arg(2));

  $variables['wrap_attributes'] = array('class' => array('l-wrapper'));

  if ($arg0 == 'user' && !is_numeric($arg1)) {
    $variables['tabs'] = FALSE;
  }
  // Special handling for header image.
  if ($arg0 == 'user' && is_numeric($arg1) && !$arg2) {
    // Check to see if there is a profile image.
    $account = user_load($arg1); // Entity cache should save us here?
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
 * Preprocess header templates.
 * @see header.tpl.php
 */
function borg_preprocess_header(&$variables) {
  $path = backdrop_get_path('theme', 'borg');
  $variables['logo'] = theme('image', array('uri' => $path . '/logo-inverse.png'));
  // Remove Backdrop CMS from the site name in the header template.
  if ($variables['site_name'] && strstr($variables['site_name'], 'Backdrop CMS')) {
    $variables['site_name'] = trim(str_replace('Backdrop CMS', '', $variables['site_name']));
  }
}

/**
 * Preprocess views exposed form templates.
 * @see views-exposed-form.tpl.php
 */
function borg_preprocess_views_exposed_form(&$variables) {
  if (substr($variables['form']['#id'], 0, 26) == 'views-exposed-form-modules'){
    // Update search field
    $search_field_key = '';
    $search_type = check_plain(arg(0));

    if (!empty($variables['form']['keys'])){
      $search_field_key = 'keys';
    }
    elseif (!empty($variables['form']['title'])){
      $search_field_key = 'title';
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
 * Prepare variables for node templates.
 * @see node.tpl.php
 */
function borg_preprocess_node(&$variables){
  // Add missing node type suggestion.
  array_unshift($variables['theme_hook_suggestions'], 'node__' . $variables['node']->type);

  // Add theme hook suggestions for view mode.
  if ($variables['view_mode'] != 'full') {
    array_unshift($variables['theme_hook_suggestions'], 'node__' . $variables['view_mode']);
  }

  // For blog posts.
  if ($variables['type'] == 'post') {
    // Load the author.
    $author = user_load($variables['uid']);
    $lang = $author->langcode;
    // Change the username to a real name.
    if (!empty($author->field_name[$lang])) {
      $variables['name'] = l($author->field_name[$lang][0]['safe_value'], 'user/' . $author->uid);
    }

    // Get the profile photo if the field exists.
    $variables['user_picture'] = '';
    if (property_exists($author, 'field_photo')) {
      if (!empty($author->field_photo)) {
        $uri = $author->field_photo[$lang][0]['uri'];
        $variables['user_picture'] = theme('image_style', array(
          'style_name' => 'headshot_small', 'uri' => $uri));
      }
    }
  }

  // Change the submitted by language for all nodes.
  $variables['submitted'] = t('Posted by !username on !datetime', array(
    '!username' => $variables['name'], '!datetime' => $variables['date']));
}

/**
 * Prepare variables for comment templates.
 * @see comment.tpl.php
 */
function borg_preprocess_comment(&$variables) {
  // Change text to "Comment from".
  $variables['submitted'] = str_replace('Submitted by', 'Comment from', $variables['submitted']);
  // Get the headshot photo from the field.
  $author = user_load($variables['comment']->uid);
  if (!empty($author->field_photo)) {
    $langcode = $author->langcode;
    $uri = $author->field_photo[$langcode][0]['uri'];
    $variables['user_picture'] = theme('image_style', array('style_name' => 'headshot_small', 'uri' => $uri));
  }
}

/**
 * Prepare variables for block templates.
 * @see block.tpl.php
 */
function borg_preprocess_block(&$variables) {
  if ($variables['block']->module == 'system') {
    if ($variables['block']->delta == 'main-menu') {

      $icon_size = '26px';
      $icon_attributes = array('width' => $icon_size, 'height' => $icon_size);
      $icon_options = array('attributes' => $icon_attributes);
      $link_options = array('html' => TRUE, 'attributes' => array('class' => array('has-submenu')));
      $account_button = l(icon('user-circle', $icon_options), 'user', $link_options);

      global $user;
      if ($user->uid == 0) {
        $user_links = array(
          '#theme' => 'links',
          '#links' => array(
            'profile' => array(
              'title' => 'Log In',
              'href' => 'user/login',
            ),
          ),
        );
        if (user_register_access()) {
          $user_links['#links']['register'] = array(
            'title' => 'Create an account',
            'href' => 'user/register',
          );
        }
      }
      else {
        $user_links = array(
          '#theme' => 'links',
          '#links' => array(
            'profile' => array(
              'title' => 'My Profile',
              'href' => 'user',
            ),
            'logout' => array(
              'title' => 'Log out',
              'href' => 'user/logout',
            ),
          ),
        );
      }

      $inner_user_menu = backdrop_render($user_links);
      $user_menu = array(
        '#theme' => 'borg_list',
        '#attributes' => array(
          'class' => array(
            'sm',
            'menu-dropdown',
            'closed',
            'sm-nowrap',
          ),
          'data-menu-style' => 'dropdown',
        ),
        '#items' => array(
          'account' => array(
            'data' => $account_button . $inner_user_menu,
            'attributes' => array('class' => array('has-children')),
          ),
        ),
      );

      // Create a renderable containing links.

      $demo_button = l(t('Try Backdrop CMS'), 'https://backdropcms.org/try-backdrop', $link_options);
      $demo_menu = $demo_button;
      if ($version_info = _borg_get_version()) {
        $demo_links = array(
          '#theme' => 'links',
          '#links' => array(
            'demo' => array(
              'title' => 'Demo Backdrop CMS',
              'href' => 'https://www.backdropmcs.org/demo',
            ),
            'download' => array(
              'title' => 'Download Backdrop v' . $version_info['latest']['version'],
              'href' => $version_info['latest']['download_link'],
            ),
            'more' => array(
              'title' => 'Other ways to try',
              'href' => 'https://backdropcms.org/try-backdrop',
            ),
          ),
        );
        $inner_demo_menu = backdrop_render($demo_links);
        $demo_menu = array(
          '#theme' => 'borg_list',
          '#attributes' => array(
            'class' => array(
              'sm',
              'menu-dropdown',
              'closed',
              'sm-nowrap',
            ),
            'data-menu-style' => 'dropdown',
          ),
          '#items' => array(
            'demo' => array(
              'data' => $demo_button . $inner_demo_menu,
              'attributes' => array('class' => array('has-children')),
            ),
          ),
        );
      }

      $variables['account'] = $user_menu;
      $variables['demo'] = $demo_menu;

    }
  }
}

/**
 * Prepares variables for all RSS rows.
 */
function borg_preprocess_views_view_row_rss(&$variables) {
  $view = &$variables['view'];
  $item = &$variables['row'];

  // Un-escpape the previously escaped title to prevent double escaping.
  $variables['title'] = decode_entities($item->title);

  // Add a special class to the featured image to optimize for Feedly.
  $view->result[0]->field_field_image[0]['rendered']['#item']['attributes']['class'] = array('webfeedsFeaturedVisual');
  // Add an image tag to the top of the description.
  $image = backdrop_render($view->result[0]->field_field_image[0]['rendered']);
  $complete_description = '<![CDATA[' . $image . '<br/>' . $item->description . ']]>';

  $item->description = $complete_description;
  $variables['description'] = $complete_description;
}

/**
 * Prepares variables for views grid templates.
 * @see views-view-grid.tpl.php
 */
function borg_preprocess_views_view_grid(&$variables) {
  $view = $variables['view'];

  // Add bootstrap grid instead of legacy table.
  if ($view->style_plugin->options['deprecated_table']) {
    $cols     = $view->style_plugin->options['columns'];
    $rows     = $variables['rows'];

    // These views have the columns stay wider at smaller screensizes.
    $sm_grid_views = array(); // @todo

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
}

/**
 * Prepares variables for book navigation templates.
 * @see book-navigation.tpl.php
 */
function borg_preprocess_book_navigation(&$variables) {
  $book_link = $variables['book_link'];

  if ($book_link['mlid']) {
    // Change the previous link.
    if ($previous = _borg_book_prev($book_link)) {
      $previous_href = url($previous['href']);
      backdrop_add_html_head_link(array('rel' => 'prev', 'href' => $previous_href));
      $variables['prev_url'] = $previous_href;
      $variables['prev_title'] = check_plain($previous['title']);
    }
    // Change the next link.
    if ($next = _borg_book_next($book_link)) {
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

/******************************************************************************
 * Theme function overrides
 ******************************************************************************/

/**
 * Replaces book_prev().
 */
function _borg_book_prev($book_link) {
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

  $current = NULL;
  foreach ($flat as $key => $current) {
    if ($key != $book_link['mlid']) {
      $previous = $current;
    }
  }

  return $previous;
}

/**
 * Replaces book_next().
 */
function _borg_book_next($book_link) {
  $flat = book_get_flat_menu($book_link);
  // Remove child pages from next/prev links.
  foreach ($flat as $key => $item) {
    if ($item['depth'] > 2) {
      unset($flat[$key]);
    }
  }

  foreach ($flat as $key => $current) {
    if ($key == $book_link['mlid']) {
      return current($flat);
    }
  }
}

/**
 * Overrides theme_form_element().
 */
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
 * Overrides theme_socialfield_drag_components().
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
 * Overrides theme_feed_icon().
 */
function borg_feed_icon($variables) {
  $text = t('Subscribe to !feed-title', array('!feed-title' => $variables['title']));
  $image = '<i class="fa fa-rss-square"></i><span class="element-invisible">' . $text . '</span>';
  return l($image, $variables['url'], array('html' => TRUE, 'attributes' => array('class' => array('feed-icon'), 'title' => $text)));
}

/**
 * Overrides theme_menu_local_tasks().
 */
function borg_menu_local_tasks($variables) {
  $arg0 = check_plain(arg(0));
  $arg1 = check_plain(arg(1));
  $arg2 = check_plain(arg(2));
  $output = '';

  if (!empty($variables['primary'])) {
    // Remove the releases tab.
    if ($arg0 == 'node' && is_numeric($arg1) && !$arg2) {
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
function borg_on_the_web_item($variables) {
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

  if ($type == 'anchor') {
    // Add a new link class for SVG masks.
    $link_classes[] = 'otw-svg-mask';
  }

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
    $style = '';
    //$style = 'background: transparent url(' . $icon . ') no-repeat top left;';
    //$style .= ' -webkit-mask-image: url(' . $icon . ');';
    $style .= ' mask-image: url(' . $icon . ');';
    $attributes['style'] = $style;
  }

  $options = array('attributes' => $attributes, 'html' => TRUE);
  return l($text, $link, $options);
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

/**
 * Override theme_pager_link().
 */
function borg_pager_link($variables) {
  $text = $variables['text'];
  $page_new = $variables['page_new'];
  $element = $variables['element'];
  $parameters = $variables['parameters'];
  $attributes = $variables['attributes'];

  $page = isset($_GET['page']) ? $_GET['page'] : '';
  if ($new_page = implode(',', pager_load_array($page_new[$element], $element, explode(',', $page)))) {
    $parameters['page'] = $new_page;
  }

  $query = array();
  if (count($parameters)) {
    $query = backdrop_get_query_parameters($parameters, array());
  }
  if ($query_pager = pager_get_query_parameters()) {
    $query = array_merge($query, $query_pager);
  }

  // Set new pager link text
  static $pager_pieces = NULL;
  if (!isset($pager_pieces)) {
    $pager_pieces = array(
      t('« first') => array(
        'title_attribute' => t('Go to first page'),
        'before' => '« ',
        'text' => t('first'),
      ),
      t('‹ previous') => array(
        'title_attribute' => t('Go to previous page'),
        'before' => '‹ ',
        'text' => t('previous'),
      ),
      t('next ›') => array(
        'title_attribute' => t('Go to next page'),
        'text' => t('next'),
        'after' => ' ›',
      ),
      t('last »') => array(
        'title_attribute' => t('Go to last page'),
        'text' => t('last'),
        'after' => ' »',
      ),
    );
  }

  // Set the title attribute for each pager link.
  if (!isset($attributes['title'])) {
    if (isset($pager_pieces[$text])) {
      $attributes['title'] = $pager_pieces[$text]['title_attribute'];
    }
    elseif (is_numeric($text)) {
      $attributes['title'] = t('Go to page @number', array('@number' => $text));
    }
  }

  // @todo l() cannot be used here, since it adds an 'active' class based on the
  //   path only (which is always the current path for pager links). Apparently,
  //   none of the pager links is active at any time - but it should still be
  //   possible to use l() here.
  // @see http://drupal.org/node/1410574
  $attributes['href'] = url($_GET['q'], array('query' => $query));

  // How to tell when spans and new text are needed.
  $has_text = FALSE;
  if (isset($pager_pieces[$text]['text'])) {
    $has_text = TRUE;
  }

  $output  = '<a' . backdrop_attributes($attributes) . '>';
  $output .=   isset($pager_pieces[$text]['before']) ? $pager_pieces[$text]['before'] : '';
  $output .=   $has_text ? '<span class="pager-text">' : '';
  $output .=   $has_text ? check_plain($pager_pieces[$text]['text']) : check_plain($text);
  $output .=   $has_text ? '</span>' : '';
  $output .=   isset($pager_pieces[$text]['after']) ? $pager_pieces[$text]['after']: '';
  $output .= '</a>';

  return $output;
}

/**
 * Overrides theme_system_powered_by().
 */
function borg_system_powered_by() {
  $output = '<div class="drop-lounging"></div>';
  $output .= '<span>';
  $output .= t('Powered by <a href="@poweredby">Backdrop CMS</a>', array('@poweredby' => 'https://backdropcms.org'));
  $output .= '</span>';

  return $output;
}

/*******************************************************************************
 * Helper functions.
 ******************************************************************************/

/**
 * Helper function, get core version from JSON API.
 */
function _borg_get_version() {
  $cached = cache_get('backdrop_core_version_latest');
  $data = isset($cached->data) ? $cached->data : array();
  if (empty($data)) {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type: application/json'));
    curl_setopt ($ch, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)");
    curl_setopt($ch, CURLOPT_URL, 'https://backdropcms.org/core/latest.json');
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 1);
    $json = curl_exec($ch);
    $res_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
    curl_close($ch);
    $data = json_decode($json, TRUE);

    $expires = time() + 60*60; // Expire in no less than 1 hour.
    cache_set('backdrop_core_version_latest', $data, 'cache', $expires);
  }

  return $data;
}