<?php

class CRM_Sendgrid_Utils {

  /**
   * Look up a mailing ID by job ID.
   * @param int $jobId
   *   The ID number of the job.
   *
   * @return int
   *   The ID number of the corresponding mailing
   */
  public static function mailingIdFromJob($jobId) {
    $cachedMailingIDs = Civi::cache()->get('sendgridMailingIds') ?: array();
    if (empty($cachedMailingIDs[$jobId])) {
      try {
        $cachedMailingIDs[$jobId] = civicrm_api3('MailingJob', 'getvalue', array(
          'return' => "mailing_id",
          'id' => $jobId,
        ));
        Civi::cache()->get('sendgridMailingIds', $cachedMailingIDs);
      }
      catch (CiviCRM_API3_Exception $e) {
        $error = $e->getMessage();
        CRM_Core_Error::debug_log_message(ts('API Error: %1', array(
          1 => $error,
          'domain' => 'com.aghstrategies.sendgrid',
        )));
      }
    }
    return $cachedMailingIDs[$jobId];
  }

  /**
   * Look up a mailing by the job ID.
   *
   * @param int $jobId
   *   The ID number of a job.
   * @return array
   *   The result of Mailing.getsingle.
   */
  public static function getMailingByJob($jobId) {
    $mailingId = self::mailingIdFromJob($jobId);
    $mailingCache = Civi::cache()->get('sendgridMailingCache') ?: array();
    if (empty($mailingCache[$mailingId])) {
      $mailingCache[$mailingId] = civicrm_api3('Mailing', 'getsingle', array('id' => $mailingId));
      Civi::cache()->set('sendgridMailingCache', $mailingCache);
    }
    return $mailingCache[$mailingId];
  }

  public static function getSettings() {
    $settings = Civi::cache()->get('sendgridSettings');

    if (empty($settings)) {
      $settings = array(
        'secretcode' => NULL,
        'open_click_processor' => NULL,
      );
      foreach ($settings as $setting => $val) {
        try {
          $settings[$setting] = civicrm_api3('Setting', 'getvalue', array(
            'name' => "sendgrid_$setting",
            'group' => 'Sendgrid Preferences',
          ));
        }
        catch (CiviCRM_API3_Exception $e) {
          $error = $e->getMessage();
          CRM_Core_Error::debug_log_message(ts('API Error: %1', array(
            1 => $error,
            'domain' => 'com.aghstrategies.sendgrid',
          )));
        }
      }
      Civi::cache()->set('sendgridSettings', $settings);
    }
    return $settings;
  }

  public static function saveSettings($settings) {
    $existingSettings = Civi::cache()->get('sendgridSettings');
    $settingsToSave = array();

    foreach ($settings as $k => $v) {
      $existingSettings[$k] = $v;
      $settingsToSave["sendgrid_$k"] = $v;
    }
    try {
      $settingsSaved = civicrm_api3('Setting', 'create', $settingsToSave);
    }
    catch (CiviCRM_API3_Exception $e) {
      $error = $e->getMessage();
      CRM_Core_Error::debug_log_message(ts('API Error: %1', array(
        1 => $error,
        'domain' => 'com.aghstrategies.sendgrid',
      )));
    }
  }

}
