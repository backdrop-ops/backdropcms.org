<?php
/**
 * @file
 * PHP functions for the drawer layout.
 */

/**
 * Prepare variables for the drawer layout template file.
 */
function template_preprocess_layout__drawer(&$variables) {
  if ($variables['content']['sidebar'] && $variables['content']['drawer']) {
    $variables['classes'][] = 'layout-both-sidebars';
  }
  elseif ($variables['content']['sidebar'] || $variables['content']['drawer']) {
    $variables['classes'][] = 'layout-one-sidebar';

    if ($variables['content']['sidebar']) {
      $variables['classes'][] = 'layout-has-sidebar';
    }
    else {
      $variables['classes'][] = 'layout-has-drawer';
    }
  }
  else {
    $variables['classes'][] = 'layout-no-sidebars';
  }
}
