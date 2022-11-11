<?php
use CRM_Geocoder_ExtensionUtil as E;

/**
 * Geocoder.create API specification (optional)
 * This is used for documentation and validation.
 *
 * @param array $spec description of fields supported by this API call
 * @return void
 * @see http://wiki.civicrm.org/confluence/display/CRMDOC/API+Architecture+Standards
 */
function _civicrm_api3_geocoder_create_spec(&$spec) {
  $spec['is_active']['api.default'] = TRUE;
}

/**
 * Geocoder.create API
 *
 * @param array $params
 * @return array API result descriptor
 * @throws API_Exception
 */
function civicrm_api3_geocoder_create($params) {
  $result = _civicrm_api3_basic_create(_civicrm_api3_get_BAO(__FUNCTION__), $params, 'Geocoder');
  CRM_Utils_Geocode_Geocoder::resetGeoCoders();
  return $result;
}

/**
 * Geocoder.delete API
 *
 * @param array $params
 *
 * @return array API result descriptor
 * @throws \API_Exception
 * @throws \CiviCRM_API3_Exception
 * @throws \Civi\API\Exception\UnauthorizedException
 */
function civicrm_api3_geocoder_delete($params) {
  return _civicrm_api3_basic_delete(_civicrm_api3_get_BAO(__FUNCTION__), $params);
}

/**
 * Geocoder.get API
 *
 * @param array $params
 *
 * @return array API result descriptor
 */
function civicrm_api3_geocoder_get($params) {
  return _civicrm_api3_basic_get(_civicrm_api3_get_BAO(__FUNCTION__), $params);
}
