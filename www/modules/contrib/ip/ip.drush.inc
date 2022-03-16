<?php
/*
 * @file
 * Drush commands.
 */

/**
 * Implements hook_drush_command().
 */
function ip_drush_command() {

  $items['ip-backlog-comments'] = array(
    'description' => 'Insert comments IP address data loaded prior to module instalation in IP Post table taken from comments table.',
    'aliases' => array('ip-bc'),
    'options' => array(
      'size' => array(
        'description' => 'The number of entities to process in a single batch request.',
        'example-value' => '500',
      ),
    ),
  );

  $items['ip-backlog-nodes'] = array(
    'description' => 'Insert nodes IP address data loaded prior to module instalation in IP Post table from taken watchdog table (dblog).',
    'aliases' => array('ip-bn'),
    'options' => array(
      'size' => array(
        'description' => 'The number of entities to process in a single batch request.',
        'example-value' => '500',
      ),
    ),
  );

  return $items;
}

/**
 * Drush command: ip backlog comments.
 */
function drush_ip_backlog_comments() {

  $batch_size = drush_get_option('size', 500);
  $list = _drush_ip_backlog_comments_list();
  $chunks = array_chunk($list, $batch_size);

  $total = count($list);
  $progress = 0;
  $operations = array();
  foreach ($chunks as $chunk) {
    $progress += count($chunk);

    $operations[] = array(
      '_drush_ip_backlog_comments_process_batch',
      array(
        $chunk,
        dt('@percent% (Processing @progress of @total)', array(
          '@percent' => round(100 * $progress / $total),
          '@progress' => $progress,
          '@total' => $total,
        )),
      ),
    );
  }

  $batch = array(
    'operations' => $operations,
    'title' => dt('IP backlog comments process batch'),
    'finished' => '_drush_ip_backlog_comments_process_batch_finished',
    'progress_message' => dt('@current entities of @total were processed.'),
  );

  // Get the batch process all ready!
  batch_set($batch);

  // Start processing the batch operations.
  drush_backend_batch_process();
}

/**
 * Get comments list (not int ip_posts).
 */
function _drush_ip_backlog_comments_list() {
  $list = array();
  if (module_exists('comment')) {
    $list = db_query(
      "SELECT c.cid AS cid, c.hostname AS hostname" .
      " FROM {comment} c" .
      " LEFT JOIN {ip_posts} i" .
      " ON i.type = 'comment'" .
      " AND (c.cid = i.id OR i.id IS NULL)" .
      " WHERE i.id IS NULL" .
      " ORDER BY c.cid DESC"
    )->fetchAll();
  }
  return $list;
}

/**
 * Batch process: backlog comments.
 */
function _drush_ip_backlog_comments_process_batch($chunk, $details, &$context) {
  $context['message'] = $details;
  // Make sure to only initialize the results the first time.
  if (!isset($context['results']['success'])) {
    $context['results']['success'] = $context['results']['error'] = 0;
  }
  foreach ($chunk as $item) {
    $success = _drush_ip_backlog_comments_process_batch_task($item);
    $success ? $context['results']['success']++ : $context['results']['error']++;
  }
}

/**
 * Batch process task: backlog comments.
 */
function _drush_ip_backlog_comments_process_batch_task($item) {
  $success = TRUE;

  if (!ip_posts_load('comment', $item->cid)) {
    try {
      ip_posts_insert('comment', $item->cid, $item->hostname);
    }
    catch (PDOException $e) {
      $success = FALSE;
    }
  }

  return $success;
}

/**
 * Batch process finished: backlog comments.
 */
function _drush_ip_backlog_comments_process_batch_finished($success, $results, $operations) {
  if ($success) {
    // Let the user know we have finished.
    drush_log(dt('@succeeded comments were processed correctly. @errored comments failed.', array(
      '@succeeded' => empty($results['success']) ? 0 : $results['success'],
      '@errored' => empty($results['error']) ? 0 : $results['error'],
    )), 'ok');
  }
}

/**
 * Drush command: ip backlog comments.
 */
function drush_ip_backlog_nodes() {

  $batch_size = drush_get_option('size', 500);
  $list = _drush_ip_backlog_nodes_list();
  $chunks = array_chunk($list, $batch_size);

  $total = count($list);
  $progress = 0;
  $operations = array();
  foreach ($chunks as $chunk) {
    $progress += count($chunk);

    $operations[] = array(
      '_drush_ip_backlog_nodes_process_batch',
      array(
        $chunk,
        dt('@percent% (Processing @progress of @total)', array(
          '@percent' => round(100 * $progress / $total),
          '@progress' => $progress,
          '@total' => $total,
        )),
      ),
    );
  }

  $batch = array(
    'operations' => $operations,
    'title' => dt('IP backlog nodes process batch'),
    'finished' => '_drush_ip_backlog_nodes_process_batch_finished',
    'progress_message' => dt('@current entities of @total were processed.'),
  );

  // Get the batch process all ready!
  batch_set($batch);

  // Start processing the batch operations.
  drush_backend_batch_process();
}

/**
 * Get watchdog (node added) list.
 */
function _drush_ip_backlog_nodes_list() {
  $list = array();
  if (module_exists('dblog')) {
    $list = db_query(
      "SELECT wid, hostname, link" .
      " FROM {watchdog}" .
      " WHERE type = 'content'" .
      " AND message = :msg",
      array(
      ':msg' => '@type: added %title.',
    )
    )->fetchAll();
  }
  return $list;
}

/**
 * Batch process: backlog nodes.
 */
function _drush_ip_backlog_nodes_process_batch($chunk, $details, &$context) {
  $context['message'] = $details;
  // Make sure to only initialize the results the first time.
  if (!isset($context['results']['success'])) {
    $context['results']['success'] = $context['results']['error'] = 0;
  }
  foreach ($chunk as $item) {
    $success = _drush_ip_backlog_nodes_process_batch_task($item);
    $success ? $context['results']['success']++ : $context['results']['error']++;
  }
}

/**
 * Batch process task: backlog nodes.
 */
function _drush_ip_backlog_nodes_process_batch_task($item) {
  $success = TRUE;

  $nid = str_replace(array('<a href="' . base_path() . 'node/', '">view</a>'), array('', ''), $item->link);

  // Check if node still exists.
  $node = db_query('SELECT * FROM {node} WHERE nid = :nid', array(':nid' => $nid))->fetch();

  if (!empty($node) && !ip_posts_load('node', $nid)) {
    try {
      ip_posts_insert('node', $nid, $item->hostname);
    }
    catch (PDOException $e) {
      $success = FALSE;
    }
  }

  return $success;
}

/**
 * Batch process finished: backlog nodes.
 */
function _drush_ip_backlog_nodes_process_batch_finished($success, $results, $operations) {
  if ($success) {
    // Let the user know we have finished.
    drush_log(dt('@succeeded nodes were processed correctly. @errored nodes failed.', array(
      '@succeeded' => empty($results['success']) ? 0 : $results['success'],
      '@errored' => empty($results['error']) ? 0 : $results['error'],
    )), 'ok');
  }
}
