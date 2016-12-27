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

  $project_search_layouts = array(
    'search_modules',
    'search_themes',
    'search_layouts',
  );

  // Add helper class and stylesheet to add-on search pages
  if (in_array($variables['layout']->name, $project_search_layouts)) {
    $variables['classes'][] = 'layout--category-project-search';
    backdrop_add_css($basis_borg_path . '/css/layout/project-search.css', $css_options);
  }
}

/**
 * Overrides theme_menu_tree().
 */
function basis_borg_menu_tree(&$variables) {
  if ($variables['theme_hook_original'] == 'menu_tree__main_menu' && $variables['depth'] == 0) {
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
