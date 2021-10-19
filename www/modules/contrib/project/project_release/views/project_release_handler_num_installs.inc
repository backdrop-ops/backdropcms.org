<?php
/**
 * @file
 * Views field handler for number of active installations of a project.
 */

class project_release_handler_num_installs extends views_handler_field {
  function render($values) {
    $nid = isset($values->nid) ? $values->nid : 0;
    if (!$nid) {
      return '';
    }

    $num_releases = db_query('
      SELECT count
      FROM {project_usage_week_project}
      WHERE nid = :nid
      ORDER BY timestamp DESC
      LIMIT 1
      ', array(':nid' => $nid))
      ->fetchField();

    return $num_releases;
  }
}
