<?php
use CRM_Gdpr_ExtensionUtil as E;

/**
 * Contact.Anonymize API specification (optional)
 * This is used for documentation and validation.
 *
 * @param array $spec description of fields supported by this API call
 * @return void
 * @see http://wiki.civicrm.org/confluence/display/CRMDOC/API+Architecture+Standards
 */
function _civicrm_api3_contact_Anonymize_spec(&$spec) {
  $spec['id']['api.required'] = 1;
  // Support fields for Contact.get
 // return _civicrm_api3_contact_get_spec($spec);
  // Model instead on delete op.
}

/**
 * Contact.Anonymize API
 *
 * @param array $params
 * @return array API result descriptor
 * @see civicrm_api3_create_success
 * @see civicrm_api3_create_error
 * @throws API_Exception
 */
function civicrm_api3_contact_Anonymize($params) {
  $contactID = CRM_Utils_Array::value('id', $params);

  if (!empty($params['check_permissions']) && !CRM_Contact_BAO_Contact_Permission::allow($contactID, CRM_Core_Permission::DELETE)) {
    throw new \Civi\API\Exception\UnauthorizedException('Permission denied to modify contact record');
  }
  $session = CRM_Core_Session::singleton();
  if ($contactID == $session->get('userID')) {
    throw new API_Exception('This contact record is linked to the currently logged in user account - and cannot be anonymized.');
  }
  $result = CRM_Gdpr_Utils::anonymizeContact($contactID);

  if (empty($result['error'])) {
    return civicrm_api3_create_success($result['values'], $params);
  }
  else {
     throw new API_Exception($result['error_message'], $result['error_code']);
  }
}
