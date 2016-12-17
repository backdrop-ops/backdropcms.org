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
  if ($variables['layout']->name) {
    $variables['classes'][] = 'layout--category-project-search';
    backdrop_add_css($basis_borg_path . '/css/layout/project-search.css', $css_options);
  }
}
