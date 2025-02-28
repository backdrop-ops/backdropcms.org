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

  if (backdrop_is_front_page()) {
    backdrop_add_css($path . '/css/page-front.css');

    // Load IBM Plex variable fonts.
    backdrop_add_css(
      'https://fonts.googleapis.com/css2?family=IBM+Plex+Mono:ital,wght@0,100;0,200;0,300;0,400;0,500;0,600;0,700;1,100;1,200;1,300;1,400;1,500;1,600;1,700&family=IBM+Plex+Sans:ital,wght@0,100..700;1,100..700&display=swap',
      array('type' => 'external')
    );

    // Make the icons available for use in CSS.
    $home_icons = array();
    $view_features = views_get_view_result('product_features', 'block');
    foreach ($view_features as $count => $item) {
      if ($count < 4) {
        $home_icons[] = $item->field_field_icon_class[0]['raw']['safe_value'];
      }
    }
    backdrop_add_icons($home_icons);
  }

  if (arg(0) == 'features') {
    // Make all the icons available for use in CSS.
    $home_icons = array();
    $view_features = views_get_view_result('product_features', 'block');
    foreach ($view_features as $count => $item) {
      $home_icons[] = $item->field_field_icon_class[0]['raw']['safe_value'];
    }
    backdrop_add_icons($home_icons);
  }
}

/**
 * Preare varibles for node.tpl.php.
 */
function backdropcms_preprocess_node(&$variables) {
  if ($variables['type'] == 'feature') {
    $variables['icon'] = '';
    $node = $variables['node'];
    if (property_exists($node, 'field_icon_class') && !empty($node->field_icon_class)) {
      $name = $node->field_icon_class[LANGUAGE_NONE][0]['safe_value'];
      $variables['icon'] = icon('$name');
      backdrop_add_icons(array($name));
    }
  }
  // For showcase nodes include a special stylesheet.
  if ($variables['type'] == 'showcase') {
    backdrop_add_css($path . '/css/node-showcase.css');
  }
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
}

/**
 * Implements template_preprocess_block().
 */
function backdropcms_preprocess_block(&$variables) {
  if ($variables['title'] == 'Latest News') {
    // Massively simplifying the markup of this block so CSS layout is easier to accomplish
    // Getting the single result from the view, and rendering it without views markup bloat
    $data = views_get_view_result('news', 'block');

    $content =
      '<h3 class="latest-news__title">' . $data[0]->node_title . '</h3>' .
      '<div class="latest-news__body">' .
        $data[0]->field_body[0]['rendered']['#markup'] .
      '</div>' .
      '<div class="latest-news__image">' .
        render($data[0]->field_field_image[0]['rendered']) .
      '</div>';

    $variables['content'] = $content;
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

  if ($menu_name === 'main-menu' && (
        $element['#href'] == 'https://forum.backdropcms.org' ||
        $element['#href'] == 'https://docs.backdropcms.org' ||
        $element['#href'] == 'https://events.backdropcms.org')) {
    $title = check_plain($element['#title']);
    $element['#title'] = $title . ' <i class="fa fa-external-link" aria-hidden="true"></i>';
    $element['#localized_options']['html'] = TRUE;
  }

  $output = l($element['#title'], $element['#href'], $element['#localized_options']);
  return '<li' . backdrop_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
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
