<?php

/**
 * @file
 * Admin pages for Project Release module.
 */

/**
 * An admin page with a button to regenerate the project XML for all projects.
 */
function project_release_admin_regenerate_form($form, &$form_state) {
  $form['help'] = array(
    '#type' => 'help',
    '#markup' => t('Deleting all project release XML files will cause them to be generate next time they are requested. Note this can cause substantial load as new files are generated. This should only be done if all XML release files need to be updated.'),
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Regenerate Release XML',
    '#attributes' => array('class' => array('button-danger')),
  );

  $form['#submit'][] = 'project_release_admin_regenerate_form_submit';

  return $form;
}

/**
 * Set up the batch to regenerate the XML.
 */
function project_release_admin_regenerate_form_submit(&$form, &$form_state) {
  backdrop_set_message(t('Project XML updated.'), 'status');

  $project_nids = db_select('project_release', 'pr')
    ->fields('pr', array('project_nid'))
    ->distinct()
    ->execute()
    ->fetchAll(PDO::FETCH_ASSOC);

  $batch = array();
  $batch['finished'] = 'project_release_regenerate_batch_finished';
  $batch['operations'][] = array(
    'project_release_cron_xml',
    array(array('project_nid' => NULL)),
  );
  foreach($project_nids as $nid) {
    $batch['operations'][] = array(
      'project_release_cron_xml',
      array($nid)
    );
  }
  batch_set($batch);
}

/**
 * Implements batch API's 'finished' callback.
 */
function project_release_regenerate_batch_finished($success, $results, $operations) {
  $message = t('Regenerated XML for @modules.', array('@modules' => format_plural(count($results), '1 module', '@count modules')));
  backdrop_set_message($message, 'status');
}
