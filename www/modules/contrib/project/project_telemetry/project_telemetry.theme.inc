<?php

function theme_project_telemetry_allowed_values(&$variables) {
  $element = $variables['element'];

  $header = array(
    t('Key'),
    t('Label'),
    t('Aggregation precision'),
  );

  foreach (element_children($element) as $position) {
    $row = array();
    if ($position !== '_add') {
      $row[] = backdrop_render($element[$position]['key']);
      $row[] = backdrop_render($element[$position]['label']);
      $row[] = backdrop_render($element[$position]['precision']);
    }
    $rows[] = $row;
  }


  $output = '';
  $table = theme('table', array(
    'header' => $header,
    'rows' => $rows,
  ));
  $table .= backdrop_render($element['_add']);
  $wrapper = array(
    '#theme' => 'form_element',
    '#title' => t('Telemetry data details'),
    '#description' => t('Specify the allowed keys that should be saved by Project Telemetry. Keys must match the data provided by a project\'s hook_telemetry_data() keys. A human-readable label will be used when displaying aggregated data. The precision is used to group similar numbers into larger groups. For example a precision of "1 decimal" would aggregate 7.1.1 and 7.1.2 into simply 7.1. If data collected is not numeric, use "No adjustment".'),
    '#children' => $table,
  );
  $output .= backdrop_render($wrapper);
  return $output;
}
