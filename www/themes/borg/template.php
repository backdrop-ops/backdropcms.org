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
  backdrop_add_css('https://fonts.googleapis.com/css?family=Source+Sans+Pro:200,300,400,600,700', array('type' => 'external'));
  // Add FontAwesome.
  backdrop_add_js('https://use.fontawesome.com/baf3c35582.js', array('type' => 'external'));

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

  // Add a node class based on the node ID...
  if ($arg0 == 'node' && is_numeric($arg1) && !$arg2) {
    $variables['classes'][] = 'node-' . $arg1;
  }

  // ...or add body classes based on args.
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

  if ($variables['view_mode'] == 'project_search') {
    $node = $variables['node']; // Nice shorthand.

    // Set column sizes for each view-mode.
    $variables['classes_col1'] = array('col-md-4', 'col-md-push-8');
    $variables['classes_col2'] = array('col-md-8', 'col-md-pull-4');

    if ($node->type == 'project_theme') {
      $variables['classes_col1'] = array('col-md-6', 'col-md-push-6');
      $variables['classes_col2'] = array('col-md-6', 'col-md-pull-6');
    }

    // Move the image into the sidebar.
    $variables['image'] = backdrop_render($variables['content']['field_image']);

    // Start a place for footer info.
    $footer_links = array();

    // Add some statistics info for the footer.
    if (isset($variables['content']['field_download_count'])) {
      $count = backdrop_render($variables['content']['field_download_count']);
      $footer_links['count']['data'] = $count;
    }
    if (isset($variables['content']['project_usage'])) {
      $variables['content']['project_usage']['#weight'] = 10;
      $usage = backdrop_render($variables['content']['project_usage']);
      $footer_links['usage']['data'] = $usage;
    }

    // Add a more info link to content.
    $variables['content']['more'] = array(
      '#type' => 'link',
      '#title' => t('More details'),
      '#href' => url('node/' . $node->nid, array('absolute' => TRUE)),
      '#attributes' => array('class' => array('button', 'button-small', 'more-details')),
      '#weight' => 10,
    );

    // Get the recomended release info.
    $release = FALSE;
    $result = views_get_view_result('project_release_download_table', 'recommended', $node->nid);
    if (count($result) == 1) {
      $release = reset($result);
    }

    if ($release) {
      // Add the latest release version.
      $version = array(
        '#type' => 'markup',
        '#markup' => '<span>' . t('Version: <strong>@version</strong>', array('@version' => $release->project_release_node_version)) . '</span>',
      );
      $footer_links['version']['data'] = backdrop_render($version);

      // Add the latest release date.
      $date = format_date($release->node_project_release_supported_versions_created, 'short');
      $latest = array(
        '#type' => 'markup',
        '#markup' => '<span class="release-date">' . t('Released: <strong>@date</strong>', array('@date' => $date)) . '</span>',
      );
      $footer_links['latest']['data'] = backdrop_render($latest);

      // Add an area for download info.
      $variables['download'] = array();
      // Add download link.
      $variables['download']['button'] = array(
        '#type' => 'link',
        '#title' => t('Download'),
        '#href' => $release->project_release_node_download_link,
        '#attributes' => array('class' => array('button', 'button-small')),
        '#weight' => -11,
      );
      // Add download file size.
      $variables['download']['size'] = array(
        '#type' => 'markup',
        '#markup' => '<span class="download-size"><span>' . format_size($release->project_release_node_download_size) . '</span></span>',
        '#weight' => -10,
      );
    }

    // Put release info in a list.
    $variables['footer_info'] = array(
      '#theme' => 'item_list',
      '#items' => $footer_links,
    );

  }

  if ($variables['type'] == 'project_module' || $variables['type'] == 'project_theme' || $variables['type'] == 'project_layout') {

    if ($variables['view_mode'] == 'teaser') {
      if (isset($variables['content']['links'])) {
        $old_title = $variables['content']['links']['node']['#links']['node-readmore']['title'];
        $new_title = str_replace('Read more', 'More details', $old_title);
        unset($variables['content']['links']);
      }
    }
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

  // Get the theme location.
  $path = backdrop_get_path('theme', 'borg');

  // For project nodes include a special stylesheet.
  if (($variables['type'] == 'core') || substr($variables['type'], 0, 8) == 'project_'){
    if ($variables['type'] == 'project_release') {

    }
    else {
      unset($variables['content']['project_release_downloads']['#prefix']);
      $variables['classes'][] = 'node-project';
      backdrop_add_css($path . '/css/node-project.css');
    }
  }

  // For showcase nodes include a special stylesheet.
  if ($variables['type'] == 'showcase') {
    backdrop_add_css($path . '/css/node-showcase.css');
  }
}

/**
 * Prepare variables for comment templates.
 * @see comment.tpl.php
 */
function borg_preprocess_comment(&$variables){
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
 * Overrides theme_field__body__docs().
 */
function borg_field__body__docs($variables) {
  // Only add bug squad members on the bug squad page.
  if ($variables['element']['#object']->nid == '2306') {
    // Safety check for the project metrics module.
    if (module_exists('borg_project_metrics')) {
      $bug_squad = borg_project_metrics_teams('3489194');
      $members = array();
      foreach ($bug_squad as $key => $member) {
        $info  = '<img class="gh-avatar" src="' . $member['avatar_url'] . '" />';
        $info .= '<strong>' . $member['name'] . '</strong>';
        $members[] = $info;
      }

      $output  = backdrop_render($variables['element'][0]);
      $output .= '<p class="bug-squad-header"><strong>Bug Squad Members</strong></p>';
      $output .= '<div class="container">';
      $output .= '  <div class="row">';
      $output .=      theme('item_list', array('items' => $members, 'attributes' => array('class' => array('leadership', 'bug-squad'))));
      $output .= '  </div> <!-- /.row -->';
      $output .= '</div> <!-- /.container -->';

      return $output;
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
 * Overrides theme_menu_tree().
 */
function borg_menu_tree__user_menu($variables) {
  $variables['attributes']['class'][] = 'closed';

  global $user;

  $output  = '<nav class="borg-greeting">';
  $output .= '  <ul class="borg-user-menu">';
  $output .= '    <li class=top>';

  if ($user->uid) {
    $greeting = t('Hi @name!', array('@name'  => $user->name));
    $output .= '      <a href="#" id="greeting" class="greeting">' . $greeting . '</a>';
  }
  else {
    $output .= '      <a href="#" id="greeting" class="greeting">' . t('Welcome!') . '</a>';
  }

  $output .= '      <ul' . backdrop_attributes($variables['attributes']) . '>' . $variables['tree'] . '</ul>';
  $output .= '    </li>';
  $output .= '  </ul>';

  $output .= '  <a class="icon" title="Find us on GitHub" href="https://github.com/backdrop/backdrop"><i class="fa fa-github fa-2x" aria-hidden="true"></i></a>';
  $output .= '  <a class="icon" title="Follow us on Twitter" href="https://twitter.com/backdropcms"><i class="fa fa-twitter fa-2x" aria-hidden="true"></i></a>';
  $output .= '  <a class="icon" title="Subscribe to our Newsletter" href="https://backdropcms.org/newsletter"><i class="fa fa-envelope fa-2x" aria-hidden="true"></i></a>';

  $output .= '</nav>';

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
 * Overrides theme_system_powered_by().
 */
function borg_system_powered_by() {
  $output = '<div class="drop-lounging"></div>';
  $output .= '<span>';
  $output .= t('Powered by <a href="@poweredby">Backdrop CMS</a>', array('@poweredby' => 'https://backdropcms.org'));
  $output .= '</span>';

  return $output;
}
