<?php

class CRM_Gdpr_Export {

  /**
   * @param array $ids
   */
  public static function contact($ids) {
    $session = CRM_Core_Session::singleton();
    $loggedUserID = $session->get('userID');
    $activityTypeId = CRM_Gdpr_Activity::contactExportedTypeId();
    foreach ($ids as $id) {
      $params = [
        'sequential' => 1,
        'source_record_id' => $id,
        'source_contact_id' => $loggedUserID,
        'activity_type_id' => $activityTypeId,
        'activity_date_time' => date('YmdHis'),
        'status_id' => 'Completed',
        'api.ActivityContact.create' => [
          'activity_id' => '$value.id',
          'contact_id' => $id,
          'record_type_id' => 3,
        ]
      ];
      CRM_Gdpr_Utils::CiviCRMAPIWrapper('Activity', 'create', $params);
    }
  }

  /**
   * @param array $ids
   */
  public static function activity($ids) {
    $session = CRM_Core_Session::singleton();
    $loggedUserID = $session->get('userID');
    $activityTypeId = CRM_Gdpr_Activity::activityExportedTypeId();
    $query = "SELECT DISTINCTROW
                ac.contact_id, ac.activity_id, ovt.activity_name, ovs.activity_status,
                DATE_FORMAT(a.activity_date_time, '%Y-%m-%d') activity_date
              FROM civicrm_activity a
                JOIN civicrm_activity_contact ac ON ac.activity_id = a.id
                JOIN (SELECT
                         v.value, v.label activity_name
                       FROM civicrm_option_value v
                         JOIN civicrm_option_group g ON g.id = v.option_group_id AND g.name = 'activity_type'
                     ) ovt ON ovt.value = a.activity_type_id
                JOIN (SELECT
                         v.value, v.label activity_status
                       FROM civicrm_option_value v
                         JOIN civicrm_option_group g ON g.id = v.option_group_id AND g.name = 'activity_status'
                     ) ovs ON ovs.value = a.status_id
              WHERE a.id IN (" . implode(', ', array_values($ids)) . ")
                  AND ac.record_type_id IN (2, 3)";
    $dao = CRM_Core_DAO::executeQuery($query);
    while ($dao->fetch()) {
      $params = [
        'sequential' => 1,
        'source_record_id' => $dao->activity_id,
        'source_contact_id' => $loggedUserID,
        'activity_type_id' => $activityTypeId,
        'parent_id' => $dao->activity_id,
        'activity_date_time' => date('YmdHis'),
        'status_id' => 'Completed',
        'subject' => $dao->activity_name . " (" . $dao->activity_status . ") at " . $dao->activity_date,
        'api.ActivityContact.create' => [
          'activity_id' => '$value.id',
          'contact_id' => $dao->contact_id,
          'record_type_id' => 3,
        ]
      ];
      CRM_Gdpr_Utils::CiviCRMAPIWrapper('Activity', 'create', $params);
    }
  }

  /**
   * @param $ids
   */
  public static function contribution($ids) {
    $session = CRM_Core_Session::singleton();
    $loggedUserID = $session->get('userID');
    $activityTypeId = CRM_Gdpr_Activity::contributionExportedTypeId();
    $query = "SELECT
                c.id, c.contact_id, c.total_amount, c.currency,
                DATE_FORMAT(c.receive_date, '%Y-%m-%d') contribution_date,
                ft.name financial_name, ovs.contribution_status
              FROM civicrm_contribution c
                JOIN civicrm_financial_type ft ON ft.id = c.financial_type_id
                JOIN ( SELECT
                         v.value, v.label contribution_status
                       FROM civicrm_option_value v
                         JOIN civicrm_option_group g ON g.id = v.option_group_id AND g.name = 'contribution_status'
                     ) ovs ON ovs.value = c.contribution_status_id
              WHERE c.id IN (" . implode(', ', array_values($ids)) . ")";
    $dao = CRM_Core_DAO::executeQuery($query);
    while ($dao->fetch()) {
      $params = [
        'sequential' => 1,
        'source_record_id' => $dao->id,
        'source_contact_id' => $loggedUserID,
        'activity_type_id' => $activityTypeId,
        'activity_date_time' => date('YmdHis'),
        'status_id' => 'Completed',
        'subject' => $dao->financial_name . " (" . $dao->contribution_status . ") at " . $dao->contribution_date
          . " for " . $dao->total_amount . " " . $dao->currency,
        'api.ActivityContact.create' => [
          'activity_id' => '$value.id',
          'contact_id' => $dao->contact_id,
          'record_type_id' => 3,
        ]
      ];
      CRM_Gdpr_Utils::CiviCRMAPIWrapper('Activity', 'create', $params);
    }
  }

}
