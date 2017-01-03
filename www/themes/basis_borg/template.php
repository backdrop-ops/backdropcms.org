<?php

/**
 * Implements template_preprocess_layout().
 */
function basis_borg_preprocess_layout(&$variables) {
  $basis_borg_path = backdrop_get_path('theme', 'basis_borg');
  $css_options = array(
    'group' => CSS_THEME,
    'weight' => 900
  );

  // Add homepage stylesheet to homepage
  if ($variables['is_front']) {
    backdrop_add_css($basis_borg_path . '/css/layout/front.css', $css_options);
  }

  // Add helpful CSS class to layout based on layout name
  $variables['classes'][] = backdrop_clean_css_identifier('layout--name-' . $variables['layout']->name);

  switch ($variables['layout']->name) {
    case 'search_modules':
    case 'search_themes':
    case 'search_layouts':
      $variables['classes'][] = 'layout--category-project-search';
      backdrop_add_css($basis_borg_path . '/css/layout/project-search.css', $css_options);
      break;
    case 'projects':
      backdrop_add_css($basis_borg_path . '/css/layout/project.css', $css_options);
      $variables['classes'][] = 'layout--category-project';
      break;
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
      backdrop_add_css($basis_borg_path . '/css/component/news-listing.css', $css_options);
      break;
    case 'events':
      backdrop_add_css($basis_borg_path . '/css/component/events-listing.css', $css_options);
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
