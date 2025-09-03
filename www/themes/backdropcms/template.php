<?php
/**
 * @file
 * Theme and preprocess functions for the backdropcms theme.
 */

/*******************************************************************************
 * Alter functions: modify renderable structures before used.
 ******************************************************************************/


/*******************************************************************************
 * Preprocess functions: prepare variables for templates.
 ******************************************************************************/

/**
 * Prepares variables for page templates.
 * @see page.tpl.php
 */
function backdropcms_preprocess_page(&$variables) {
  $path = backdrop_get_path('theme', 'backdropcms');
  $arg0 = check_plain(arg(0));
  $arg1 = check_plain(arg(1));
  $arg2 = check_plain(arg(2));
  $icons_needed = array();

  if (backdrop_is_front_page()) {
    backdrop_add_css($path . '/css/page-front.css', array('group' => CSS_THEME));

    // Load IBM Plex variable fonts.
    backdrop_add_css(
      'https://fonts.googleapis.com/css2?family=IBM+Plex+Mono:ital,wght@0,100;0,200;0,300;0,400;0,500;0,600;0,700;1,100;1,200;1,300;1,400;1,500;1,600;1,700&family=IBM+Plex+Sans:ital,wght@0,100..700;1,100..700&display=swap',
      array('type' => 'external')
    );
  }
  elseif ($arg0 == 'node' && is_numeric($arg1) && !$arg2) {
    $variables['classes'][] = 'node-' . $arg1;
    $node = node_load($arg1); // Entity cache should save us here.
    if (substr($node->type, 0, 8) == 'project_') {
      $icons_needed[] = 'arrow-circle-down-fill';
    }
  }
  elseif ($arg0 == 'modules' || $arg0 == 'themes' || $arg0 == 'layouts') {
    $variables['classes'][] = 'project-search';
    backdrop_add_css($path . '/css/page-project-search.css');
  }
  elseif ($arg0 == 'support') {
    if ($arg1 == 'services') {
      backdrop_add_css($path . '/css/page-services.css');
    }
  }

  // Make the icons needed available for use in CSS.
  if (!empty($icons_needed)) {
    backdrop_add_icons($icons_needed);
  }
}

/**
 * Preare varibles for node.tpl.php.
 */
function backdropcms_preprocess_node(&$variables) {
  // Get the theme location.
  $path = backdrop_get_path('theme', 'backdropcms');

  if ($variables['type'] == 'feature') {
    $variables['icon'] = '';
    $node = $variables['node'];
    if (property_exists($node, 'field_icon_class') && !empty($node->field_icon_class)) {
      $name = $node->field_icon_class[LANGUAGE_NONE][0]['safe_value'];
      $variables['icon'] = icon('$name');
      backdrop_add_icons(array($name));
      if ($variables['view_mode'] == 'card') {
        $variables['classes'][] = 'dark-hero-card--' . $name;
      }
    }
  }
  // For showcase nodes include a special stylesheet.
  if ($variables['type'] == 'showcase') {
    backdrop_add_css($path . '/css/node-showcase.css');
  }

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
    // Link text <span class="element-invisible"> about ' . check_plain($node->title) . '</span>'
    $variables['content']['more'] = array(
      '#type' => 'link',
      '#title' => t('More details'),
      '#href' => url('node/' . $node->nid, array('absolute' => TRUE)),
      '#attributes' => array('class' => array('more-details')),
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
}

/**
 * Implements template_preprocess_block().
 */
function backdropcms_preprocess_block(&$variables) {
  if ($variables['title'] == 'Latest News') {
    // Massively simplifying the markup of this block so CSS layout is easier to accomplish
    // Getting the single result from the view, and rendering it without views markup bloat
    $data = views_get_view_result('news', 'block');

    $title = $data[0]->node_title;
    if (isset($data[0]->nid)) {
      $article_url = backdrop_get_path_alias('node/' . $data[0]->nid);
      $title = '<a href="' . $article_url . '">' . $title . '</a>';
    }

    $content = [
      'title' => ['#markup' => '<h3 class="latest-news__title">' . $title . '</h3>'],
      'body' => ['#markup' =>
        '<div class="latest-news__body">' .
          $data[0]->field_body[0]['rendered']['#markup'] .
        '</div>'
      ],
      'image' => ['#markup' =>
        render($data[0]->field_field_image[0]['rendered'])
      ]
    ];

    $variables['content'] = $content;
  }
}

/**
 * Preare varibles for views templates.
 *
 * @see views-view.tpl.php.
 */
function backdropcms_preprocess_views_view(&$variables) {
  $view = $variables['view'];
  if ($view->name == 'product_features') {

    // Make the icons available for use in CSS.
    $icons_needed = array();
    foreach ($view->result as $count => $item) {
      if (!empty($item->field_field_icon_class)) {
        $icons_needed[] = $item->field_field_icon_class[0]['raw']['safe_value'];
      }
    }
    if (!empty($icons_needed)) {
      backdrop_add_icons($icons_needed);
    }
  }
}

/******************************************************************************
 * Theme function overrides
 ******************************************************************************/

/**
 * Overrides theme_menu_link().
 */
function backdropcms_menu_link(array $variables) {
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

  $external = array(
    'https://forum.backdropcms.org',
    'https://docs.backdropcms.org',
    'https://docs.backdropcms.org/documentation/user-guide',
    'https://events.backdropcms.org',
  );

  if ($menu_name === 'main-menu' && in_array($element['#href'], $external)) {
    $uri = icon_get_path('arrow-square-out');
    $svg_contents = file_get_contents($uri);
    $attributes = array('width' => '1em', 'height' => '1em');
    $svg_contents = image_add_svg_attributes($svg_contents, $attributes);

    $title = check_plain($element['#title']);
    $element['#title'] = $title . '&nbsp' . $svg_contents;
    $element['#localized_options']['html'] = TRUE;
  }

  $output = l($element['#title'], $element['#href'], $element['#localized_options']);
  return '<li' . backdrop_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}

/**
 * Overrides theme_item_list().
 * - Only adds the outer div if there is a title.
 */
function backdropcms_item_list($variables) {
  $items = $variables['items'];
  $title = $variables['title'];
  $type = $variables['type'];
  $list_attributes = $variables['attributes'];
  $wrapper_attributes = $variables['wrapper_attributes'];

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
        $attributes = array_diff_key($item, array('data' => 0, 'children' => 0));

        // Append nested child list, if any.
        if (isset($item['children'])) {
          // HTML attributes for the outer list are defined in the 'attributes'
          // theme variable, but not inherited by children. For nested lists,
          // all non-numeric keys in 'children' are used as list attributes.
          $child_list_attributes = array();
          foreach ($item['children'] as $child_key => $child_item) {
            if (is_string($child_key)) {
              $child_list_attributes[$child_key] = $child_item;
              unset($item['children'][$child_key]);
            }
          }
          $value .= theme('item_list', array(
            'items' => $item['children'],
            'type' => $type,
            'attributes' => $child_list_attributes,
          ));
        }
      }
      else {
        $value = $item;
      }

      $attributes['class'][] = ($i % 2 ? 'odd' : 'even');
      if ($i == 1) {
        $attributes['class'][] = 'first';
      }
      if ($i == $num_items) {
        $attributes['class'][] = 'last';
      }

      $output .= '<li' . backdrop_attributes($attributes) . '>' . $value . '</li>';
    }
    $output .= "</$type>";
  }
  elseif (!empty($variables['empty'])) {
    $output .= render($variables['empty']);
  }

  // Only output the list container and title if there are any list items.
  if ($output !== '') {
    // Check to see whether the list title exists before adding a header. Empty
    // headers are not semantic and present accessibility challenges.
    if (isset($title) && $title !== '') {
      $title = '<h3>' . $title . '</h3>';
    }

    // Add any attributes specified for the wrapper div tag.
    if (!isset($wrapper_attributes['class'])) {
      // Make sure that the 'class' key exists in the array.
      $wrapper_attributes['class'] = array();
    }
    elseif (is_string($wrapper_attributes['class'])) {
      // Do not choke if 'class' was provided as a string which may include
      // commas, spaces, or semicolons. Convert sub-strings into array items.
      $wrapper_class_items = array_map('trim', preg_split("/[;,]/", $wrapper_attributes['class']));
      $wrapper_attributes['class'] = $wrapper_class_items;
    }
    // Finally, include a default CSS class "item-list".
    $wrapper_attributes['class'][] = 'item-list';

    // CHANGED: only add the outer wrapper if there is a title.
    if (!empty($title)) {
      $output = '<div' . backdrop_attributes($wrapper_attributes) . '>' . $title . $output . '</div>';
    }
  }

  return $output;
}

/**
 * Overrides theme_field__body__docs().
 */
function backdropcms_field__body__docs($variables) {

  // Add bug squad members on the bug squad page.
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

  // Add security team members on the security team page.
  if ($variables['element']['#object']->nid == '2566') {
    // Safety check for the project metrics module.
    if (module_exists('borg_project_metrics')) {
      $sec_team = borg_project_metrics_teams('1817637');
      $members = array();
      foreach ($sec_team as $key => $member) {
        $info  = '<img class="gh-avatar" src="' . $member['avatar_url'] . '" />';
        $info .= '<strong>' . $member['name'] . '</strong>';
        $members[] = $info;
      }

      $output  = backdrop_render($variables['element'][0]);
      $output .= '<p class="bug-squad-header"><strong>Security Team Members</strong></p>';
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
 * Overrides theme_system_powered_by().

function backdropcms_system_powered_by() {
  return '<span>' . t('<a href="@url">Backdrop CMS</a> and the Backdrop logo are registered trademarks of the Softrware Freedom conservancy.', array('@url' => 'https://backdropcms.org')) . '</span>';
}
*/