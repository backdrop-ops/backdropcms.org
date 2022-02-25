<?php
/**
 * @php file
 * Preprocess function for Borg Sutro template.
 */

function template_preprocess_layout__borg_sutro(&$variables) {
  $variables['top_column_classes'] = array('l-top-col');

  // Count the top regions.
  $top_region_count = 0;
  if ($variables['content']['top1']) {
    $top_region_count++;
  }
  if ($variables['content']['top2']) {
    $top_region_count++;
  }
  if ($variables['content']['top3']) {
    $top_region_count++;
  }

  if ($top_region_count) {
    switch ($top_region_count) {
      case 1:
        $variables['top_column_classes'][] = 'col-md-12';
        break;
      case 2:
        $variables['top_column_classes'][] = 'col-md-6';
        break;
      case 3:
        $variables['top_column_classes'][] = 'col-md-4';
        break;
    }
  }
}

