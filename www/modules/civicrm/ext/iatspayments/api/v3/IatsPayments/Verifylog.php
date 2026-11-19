<?php

/**
 * @file
 */

/**
 * Action IatsPayments VerifyLog.
 *
 * @param array $params
 *
 * @return array
 *   API result array.
 *
 * @throws CRM_Core_Exception
 */

/**
 *
 */
function civicrm_api3_iats_payments_verifylog($params) {
  try {
    $customer_code = empty($params['customer_code']) ? '' : $params['customer_code'];
    if (!empty($params['contribution'])) {
      $contribution = $params['contribution'];
      $query_params = [
        1 => [$customer_code, 'String'],
        2 => [$contribution['contact_id'], 'Integer'],
        3 => [$contribution['id'], 'Integer'],
        4 => [$params['contribution_status_id'], 'Integer'],
        5 => [$params['transaction_id'], 'String'],
        6 => [$contribution['contribution_recur_id'], 'Integer'],
      ];
      if (empty($contribution['contribution_recur_id'])) {
        unset($query_params[6]);
        $result = CRM_Core_DAO::executeQuery("INSERT INTO civicrm_iats_verify
          (customer_code, cid, contribution_id, contribution_status_id, auth_result, verify_datetime) VALUES (%1, %2, %3, %4, %5, NOW())", $query_params);
      }
      else {
        $result = CRM_Core_DAO::executeQuery("INSERT INTO civicrm_iats_verify
          (customer_code, cid, contribution_id, contribution_status_id, auth_result, verify_datetime, recur_id) VALUES (%1, %2, %3, %4, %5, NOW(), %5)", $query_params);
      }
    }
    else {
      $query_params = [
        1 => [$customer_code, 'String'],
        2 => [$params['transaction_id'], 'String'],
      ];
      $result = CRM_Core_DAO::executeQuery("INSERT INTO civicrm_iats_verify
        (customer_code, auth_result, verify_datetime) VALUES (%1, %2, NOW())", $query_params);
    }
  }
  catch (Exception $e) {
    throw CRM_Core_Exception('iATS Payments verification logging failed.');
  }
  return civicrm_api3_create_success(TRUE, $params);
}

/**
 * Action payment.
 *
 * @param array $params
 *
 * @return array
 */
function _civicrm_api3_iats_payments_verifylog_spec(&$params) {
  $params['transaction_id']['api.required'] = 1;
}
