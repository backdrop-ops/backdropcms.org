<?php

/**
 * @file
 * Administration pages for mailchimp_automations module.
 */

/**
 * Returns a form for a mailchimp_automations_entity.
 */
function mailchimp_automations_entity_form($form, &$form_state, MailchimpAutomationsEntity $mailchimp_automations_entity = NULL, $op = 'edit') {
  if ($op == 'clone') {
    $mailchimp_automations_entity->label .= ' (cloned)';
    $mailchimp_automations_entity->name = '';
  }
  $entitynotnull = isset($mailchimp_automations_entity->mailchimp_automations_entity_id);
  $form_state['mailchimp_automation'] = $mailchimp_automations_entity;

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $entitynotnull ? $mailchimp_automations_entity->label : '',
    '#description' => t('The human-readable name of this automation entity.'),
    '#required' => TRUE,
    '#weight' => -10,
  );

  $form['name'] = array(
    '#title' => t('Name'),
    '#type' => 'machine_name',
    '#default_value' => $entitynotnull ? $mailchimp_automations_entity->name : '',
    '#description' => t('machine name should only contain lowercase letters & underscores'),
    '#disabled' => !empty($mailchimp_automations_entity->name),
    '#required' => TRUE,
    '#machine_name' => array(
      'exists' => 'mailchimp_automations_load_entities',
      'source' => array('label'),
    ),
    '#weight' => -9,
  );

  $form['backdrop_entity'] = array(
    '#title' => t('Backdrop entity'),
    '#type' => 'fieldset',
    '#attributes' => array(
      'id' => array('mailchimp-automation-backdrop-entity'),
    ),
    '#weight' => -8,
  );
  // Prep the entity type list before creating the form item:
  $entity_types = array('' => t('-- Select --'));
  foreach (entity_get_info() as $entity_type => $info) {
    // Ignore MailChimp entity types:
    if (strpos($entity_type, 'mailchimp') === FALSE) {
      $entity_types[$entity_type] = $info['label'];
    }
  }
  $automation_entity_label = $entitynotnull ? $mailchimp_automations_entity->label : '';
  backdrop_alter('mailchimp_automations_entity_options', $entity_types, $automation_entity_label);
  asort($entity_types);
  $form['backdrop_entity']['entity_type'] = array(
    '#title' => t('Entity type'),
    '#type' => 'select',
    '#options' => $entity_types,
    '#default_value' => $entitynotnull ? $mailchimp_automations_entity->entity_type : 0,
    '#required' => TRUE,
    '#description' => t('Select an entity type to trigger a mailchimp automation.'),
    '#ajax' => array(
      'callback' => 'mailchimp_automations_mapping_form_callback',
      'wrapper' => 'mailchimp-automation-backdrop-entity',
    ),
    '#weight' => -8,
  );

  $form_entity_type = & $form_state['values']['entity_type'];
  if (!$form_entity_type && $entitynotnull) {
    $form_entity_type = $mailchimp_automations_entity->entity_type;
  }
  if ($form_entity_type) {
    // Prep the bundle list before creating the form item:
    $bundles = array('' => t('-- Select --'));
    $info  = entity_get_info($form_entity_type);
    foreach ($info['bundles'] as $key => $bundle) {
      $bundles[$key] = $bundle['label'];
    }
    asort($bundles);
    $form['backdrop_entity']['bundle'] = array(
      '#title' => t('Entity bundle'),
      '#type' => 'select',
      '#required' => TRUE,
      '#description' => t('Select a Backdrop entity bundle with an email address to report on.'),
      '#options' => $bundles,
      '#default_value' => $entitynotnull ? $mailchimp_automations_entity->bundle : 0,
      '#weight' => -7,
      '#ajax' => array(
        'callback' => 'mailchimp_automations_mapping_form_callback',
        'wrapper' => 'mailchimp-automation-backdrop-entity',
      ),
    );

    $form_bundle = & $form_state['values']['bundle'];
    if (!$form_bundle && $entitynotnull) {
      $form_bundle = $mailchimp_automations_entity->bundle;
    }
    if ($form_bundle) {
      // Prep the field & properties list before creating the form item:
      $fields = mailchimp_automations_email_fieldmap_options($form_entity_type, $form_bundle);
      $form['backdrop_entity']['email_property'] = array(
        '#title' => t('Email Property'),
        '#type' => 'select',
        '#required' => TRUE,
        '#description' => t('Select the field which contains the email address'),
        '#options' => $fields,
        '#default_value' => $entitynotnull ? $mailchimp_automations_entity->email_property : 0,
        '#maxlength' => 127,
        '#weight' => -6,
        '#ajax' => array(
          'callback' => 'mailchimp_automations_mapping_form_callback',
          'wrapper' => 'mailchimp-automation-backdrop-entity',
        ),
      );

      $form_email_property = & $form_state['values']['email_property'];
      if (!$form_email_property && $entitynotnull) {
        $form_email_property = $mailchimp_automations_entity->email_property;
      }
      if ($form_email_property) {
        $mc_lists = mailchimp_get_lists();
        $options = array(t('-- Select --'));
        foreach ($mc_lists as $list_id => $list) {
          $options[$list_id] = $list->name;
        }
        if (!empty($options)) {
          $form['backdrop_entity']['list_id'] = array(
            '#title' => t('MailChimp List'),
            '#type' => 'select',
            '#required' => TRUE,
            '#description' => t('Select the list that you created in MailChimp.'),
            '#options' => $options,
            '#default_value' => $entitynotnull ? $mailchimp_automations_entity->list_id : 0,
            '#maxlength' => 127,
            '#weight' => -5,
            '#ajax' => array(
              'callback' => 'mailchimp_automations_mapping_form_callback',
              'wrapper' => 'mailchimp-automation-backdrop-entity',
            ),
          );
        }
        else {
          $msg = t('You must add at least one list in MailChimp for this functionality to work.');
          backdrop_set_message($msg, 'error', FALSE);
        }

        $form_list_id = & $form_state['values']['list_id'];
        if (!$form_list_id && $entitynotnull) {
          $form_list_id = $mailchimp_automations_entity->list_id;
        }
        if ($form_list_id) {
          $automations = mailchimp_automations_get_automations();
          if (!empty($automations)) {
            $automation_options = mailchimp_automations_options_list($automations);
            $form['backdrop_entity']['workflow_id'] = array(
              '#title' => t('Workflow ID'),
              '#type' => 'select',
              '#required' => TRUE,
              '#description' => t('Select the workflow automation that you created in MailChimp. Untitled workflows are not included.'),
              '#options' => $automation_options,
              '#default_value' => $entitynotnull ? $mailchimp_automations_entity->workflow_id : 0,
              '#maxlength' => 127,
              '#weight' => -4,
              '#ajax' => array(
                'callback' => 'mailchimp_automations_mapping_form_callback',
                'wrapper' => 'mailchimp-automation-backdrop-entity',
              ),
            );
          }
          else {
            $msg = t('You must add at least one workflow in MailChimp for this functionality to work.');
            backdrop_set_message($msg, 'error', FALSE);
          }

          $form_workflow_id = & $form_state['values']['workflow_id'];
          if (!$form_workflow_id && $entitynotnull) {
            $form_workflow_id = $mailchimp_automations_entity->workflow_id;
          }
          if ($form_workflow_id) {
            // Once the workflow id is saved, add the workflow email options.
            $emails = mailchimp_automations_get_emails_for_workflow($form_workflow_id);
            if (!empty($emails)) {
              array_unshift($emails, t('-- Select --'));
              $form['backdrop_entity']['workflow_email_id'] = array(
                '#title' => t('Workflow Email ID'),
                '#type' => 'select',
                '#required' => TRUE,
                '#description' => t('Select the workflow automation email that you created in MailChimp.'),
                '#options' => $emails,
                '#default_value' => $entitynotnull ? $mailchimp_automations_entity->workflow_email_id : 0,
                '#maxlength' => 127,
                '#weight' => -3,
              );
            }
            else {
              $msg = t('You must add at least one workflow email in MailChimp for this functionality to work.');
              backdrop_set_message($msg, 'error', FALSE);
            }
          }
        }
      }
    }
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#value' => t('Save Entity'),
    '#type' => 'submit',
  );

  return $form;
}

/**
 * Creates the options array for the select menu.
 */
function mailchimp_automations_options_list($automations) {
  $options = array(t('-- Select --'));
  foreach ($automations as $workflow) {
    $title = $workflow->settings->title;
    if (!empty($title)) {
      $options[$workflow->id] = $title;
    }
  }
  return $options;
}

/**
 * Ajax callback for mailchimp_automations_mapping_form().
 */
function mailchimp_automations_mapping_form_callback($form, &$form_state) {
  return $form['backdrop_entity'];
}

/**
 * Validation callback for mailchimp_automations_entity_form().
 */
function mailchimp_automations_entity_form_validate($form, &$form_state) {
  if ($form_state['submitted']) {
    $extant_mc_entities = entity_load('mailchimp_automations_entity');
    $form_id = $form_state['mailchimp_automations_entity']->mailchimp_automations_entity_id;
    $form_bundle = $form_state['values']['bundle'];
    $form_entity_id = $form_state['values']['entity_type'];
    foreach ($extant_mc_entities as $extant_ent) {
      if ($form_bundle == $extant_ent->bundle &&
          $form_entity_id == $extant_ent->entity_type &&
          $form_id != $extant_ent->mailchimp_automations_entity_id) {
        form_set_error(
          'bundle',
          t('A MailChimp Automation Entity already exists for this Bundle. Either select a different Bundle or edit the !link for this bundle.',
            array(
              '!link' => l(t('existing MailChimp Automation Entity'), "admin/config/services/mailchimp/automations/manage/{$extant_ent->name}"),
            )
          ));
      }
    }
  }
}

/**
 * Submit handler for mailchimp_automations_entity_form().
 */
function mailchimp_automations_entity_form_submit($form, &$form_state) {
  $dummy_entity = entity_create($form_state['values']['entity_type'], array('type' => $form_state['values']['bundle']));
  $entity_info = entity_get_info($dummy_entity->type);
  if (!empty($entity_info)) {
    // Create empty entity ID field so entity_uri doesn't result in an error.
    $entity_id_field = $entity_info['entity keys']['id'];
    $dummy_entity->$entity_id_field = NULL;
  }
  $uri = entity_uri($form_state['values']['entity_type'], $dummy_entity);
  $values = $form_state['values'];
  if ($form_state['op'] == 'add' || $form_state['op'] == 'clone') {
    $automation_entity = entity_create('mailchimp_automations_entity', $values);
  }
  else {
    $automation_entity = $form_state['mailchimp_automations_entity'];
    foreach ($values as $key => $val) {
      $automation_entity->{$key} = $val;
    }
  }
  $automation_entity->save();
  // Make sure the new items appear in the menus:
  menu_rebuild();
  $form_state['redirect'] = 'admin/config/services/mailchimp/automations';
}

/**
 * Return all possible Backdrop properties for a given entity type.
 *
 * @param string $entity_type
 *   Name of entity whose properties to list.
 * @param string $entity_bundle
 *   Optional entity bundle to get properties for.
 *
 * @return array
 *   List of entities that can be used as an #options list.
 */
function mailchimp_automations_email_fieldmap_options($entity_type, $entity_bundle = NULL) {
  $options = array('' => t('-- Select --'));

  $properties = entity_get_all_property_info($entity_type);
  if (isset($entity_bundle)) {
    $info = entity_get_property_info($entity_type);
    $properties = $info['properties'];
    if (isset($info['bundles'][$entity_bundle])) {
      $properties += $info['bundles'][$entity_bundle]['properties'];
    }
  }

  foreach ($properties as $key => $property) {
    $type = isset($property['type']) ? entity_property_extract_innermost_type($property['type']) : 'text';
    $is_entity = ($type == 'entity') || (bool) entity_get_info($type);
    // Leave entities out of this.
    if (!$is_entity) {
      if (isset($property['field']) && $property['field'] && !empty($property['property info'])) {
        foreach ($property['property info'] as $sub_key => $sub_prop) {
          $options[$property['label']][$key . ':' . $sub_key] = $sub_prop['label'];
        }
      }
      else {
        $options[$key] = $property['label'];
      }
    }
  }

  return $options;
}
