<?php

/**
 * @file 
 * Default views.
 */

/**
 * Implements hook_views_default_views().
 */
function jen_views_views_default_views() {
  $views_path = drupal_get_path('module', 'jen_views') . '/views';
  $files = file_scan_directory($views_path, '/\.view$/');

  foreach ($files as $filepath => $file) {
    require $filepath;
    if (isset($view)) {
      $views[$view->name] = $view;
    }
  }

  return $views;
}
