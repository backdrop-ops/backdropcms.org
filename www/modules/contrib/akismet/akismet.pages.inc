<?php

/**
 * @file
 * Various non-administration page callbacks for the akismet module.
 */

/**
 * Form builder for report to Akismet form.
 *
 * @param $entity
 *   The entity type of the data to report, e.g. 'node' or 'comment'.
 * @param $id
 *   The entity id the data belongs to.
 *
 * @see akismet_report_access()
 */
function akismet_report_form($form, &$form_state, $entity, $id) {
  $form['entity'] = array(
    '#type' => 'value',
    '#value' => $entity,
  );
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );
  // @todo "Delete" does not work for reporting mails to Akismet. In D7+, this
  //   form should be used solely for mails, as other entities are reported
  //   through existing delete confirmation forms instead. Perhaps there should
  //   be a dedicated form for reporting mails, as they are not really
  //   compatible with any of the standard processes either way.
  $form = confirm_form($form,
    t('Are you sure you want to delete and report the content as inappropriate?'),
    '<front>',
    t('This action cannot be undone.'),
    t('Delete'), t('Cancel')
  );
  akismet_data_delete_form_alter($form, $form_state);
  return $form;
}

/**
 * Form submit handler for akismet_report_form().
 */
function akismet_report_form_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $entity = $form_state['values']['entity'];
    $id = $form_state['values']['id'];

    // Load the Akismet session data.
    $data = akismet_data_load($entity, $id);

    // Send feedback to Akismet, if we have the original request data.
    if ((!empty($data->request)) && !empty($form_state['values']['akismet']['feedback'])) {
      if (_akismet_send_feedback($data, $form_state['values']['akismet']['feedback'])) {
        backdrop_set_message(t('The content was successfully reported as inappropriate.'));
      }
    }

    // Delete the content. The callback should take care of proper deletion and
    // cache clearing on its own.
    foreach (akismet_form_list() as $form_id => $info) {
      if (!isset($info['entity']) || $info['entity'] != $entity) {
        continue;
      }
      // If there is a 'report delete callback', invoke it.
      if (isset($info['report delete callback']) && function_exists($info['report delete callback'])) {
        $function = $info['report delete callback'];
        $function($entity, $id);
        break;
      }
    }

   $form_state['redirect'] = '<front>';
  }
}
