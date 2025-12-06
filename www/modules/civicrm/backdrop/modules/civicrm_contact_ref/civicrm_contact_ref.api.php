<?php

/**
 * @file
 * Hooks and callbacks provided by CiviCRM Contact Reference.
 */

/**
 * @addtogroup callbacks
 * @{
 */

/**
 * Callback for civicrm_contact_ref_options_list_values().
 *   The function name needs to be defined in the field's setting
 *   <code>options_list_callback</code>, used for options lists.
 *
 * @param array $field
 *   The field definition.
 * @param array $instance
 *   (optional) A field instance array. Defaults to NULL.
 * @param string $entity_type
 *   (optional) The type of entity; e.g. 'node' or 'user'. Defaults to NULL.
 * @param EntityInterface $entity
 *   (optional) The entity object. Defaults to NULL.
 *
 * @return array
 *   An options array with the contact ID as the key and the value is displayed.
 */
function callback_options_list_for_contact_reference($field, $instance, $entity_type, $entity) {
  if (!civicrm_initialize()) {
    return;
  }

  $options = array();
  $query = \Civi\Api4\Contact::get(FALSE)
    ->addSelect('sort_name', 'external_identifier');

  $contacts = $query->execute();
  if ($contacts->count() > 0) {
    foreach ($contacts as $contact) {
      $options[$contact['id']] = $contact['sort_name'] . ' (' . $contact['external_identifier'] . ') (cid:' . $contact['id'] . ')';
    }
  }

  return $options;
}

/**
 * Callback for _civicrm_contact_ref_potential_references().
 *   The function name needs to be defined in the field's setting
 *   <code>allowed_values_function</code>.
 */
function callback_allowed_values_for_contact_reference($field_name, $entity_type, $bundle_name, $entity_id = '', $string, $exact_string) {
  if (!civicrm_initialize()) {
    return;
  }

  $where = [];
  if (isset($string)) {
    if ($exact_string) {
      $where[] = ['sort_name', '=', $string];
    }
    else {
      $where[] = ['sort_name', 'LIKE', '%' . $string . '%'];
    }
  }

  $references = array();
  $query = \Civi\Api4\Contact::get(FALSE)
    ->addSelect('sort_name')
    ->setLimit(10)
    ->setWhere($where);

  $contacts = $query->execute();
  if ($contacts->count() > 0) {
    foreach ($contacts as $contact) {
      $references[$contact['id']] = array(
        'title' => $contact['sort_name'],
        'rendered' => $contact['sort_name'] . ' [cid:' . $contact['id'] . ']',
      );
    }
  }

  return $references;
}

/**
 * @} End of "addtogroup callbacks".
 */

