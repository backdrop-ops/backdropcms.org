<?php

class CRM_Airmail_Utils {

  /**
   * Wrapper for CiviCRM's translation function setting the extension domain.
   *
   * @param string $string
   *   The string to translate.
   * @param array $args
   *   Any arguments that need to be passed to `ts()`.
   *
   * @return string
   *   The translated string.
   */
  public static function ts($string, $args = []) {
    $args['domain'] = 'com.aghstrategies.airmail';
    return ts($string, $args);
  }

  /**
   * Breaks down source email address
   *
   * @param string $string
   *   String of job id hash and queue (old-school bounce address)
   *   ex: b.179.46.731d881bbb3f9aad@ex.com
   *
   * @return array
   *   Including job_id, event_queue_id, and hash.
   */
  public static function parseSourceString($string) {
    $dao = new CRM_Core_DAO_MailSettings();
    $dao->domain_id = CRM_Core_Config::domainID();
    $dao->find();
    while ($dao->fetch()) {
      // 0 = activities; 1 = bounce in this case we are just looking for bounce
      if ($dao->is_default == 1) {

        // empty array to use for preg match
        $matches = array();

        // Get Verp separator setting
        $config = CRM_Core_Config::singleton();
        $verpSeperator = preg_quote($config->verpSeparator);

        $twoDigitStringMin = $verpSeperator . '(\d+)' . $verpSeperator . '(\d+)';
        $twoDigitString = $twoDigitStringMin . $verpSeperator;
        // $string ex: b.179.46.731d881bbb3f9aad@sestest.garrison.aghstrategies.net
        // Based off of CRM/Utils/Mail/EmailProcessor.php
        $regex = '/^' . preg_quote($dao->localpart) . '(b|c|e|o|r|u)' . $twoDigitString . '([0-9a-z]{16})@' . preg_quote($dao->domain) . '$/';
        if (preg_match($regex, $string, $matches)) {
          list($match, $action, $job, $queue, $hash) = $matches;
          $bounceEvent = array(
            'job_id' => $job,
            'event_queue_id' => $queue,
            'hash' => $hash,
          );
          return $bounceEvent;
        }
      }
    }
  }

  /**
   * Listing of available backends.
   *
   * This is used to drive settings and handle incoming webhook messages.
   *
   * Each value must have `class` and `label` keys.
   *
   * @return array
   */
  public static function listBackends($optionList = FALSE) {
    $backends = [
      'SES' => [
        'class' => 'CRM_Airmail_Backend_Ses',
        'label' => 'Amazon SES',
      ],
      'SendGrid' => [
        'class' => 'CRM_Airmail_Backend_Sendgrid',
        'label' => 'SendGrid',
      ],
      'Elastic' => [
        'class' => 'CRM_Airmail_Backend_Elastic',
        'label' => 'ElasticEmail',
      ],
    ];

    if ($optionList) {
      $return = [];
      foreach ($backends as $val => $backend) {
        $return[$val] = $backend['label'];
      }
      return $return;
    }

    return $backends;
  }

  /**
   * Get the appropriate backend object.
   *
   * @return object
   *   The SMTP backend handler.
   */
  public static function getBackend() {
    $settings = self::getSettings();
    $backends = self::listBackends();
    if (empty($backends[$settings['external_smtp_service']])) {
      return NULL;
    }
    return new $backends[$settings['external_smtp_service']]['class']();
  }

  /**
   * Get Settings if Set
   *
   * @return array
   *   The key is the setting name and the value is value of setting
   */
  public static function getSettings() {
    $settings = [
      'secretcode' => NULL,
      'external_smtp_service' => NULL,
      'ee_wrapunsubscribe' => NULL,
      'ee_unsubscribe' => NULL,
    ];
    foreach (array_keys($settings) as $setting) {
      $settings[$setting] = Civi::settings()->get("airmail_{$setting}");
    }
    return $settings;
  }

  /**
   * Save airmail settings.
   *
   * @param array $settings
   *   The settings to save.
   */
  public static function saveSettings($settings) {
    foreach ($settings as $k => $v) {
      Civi::settings()->set("airmail_{$k}", $v);
    }
  }

  /**
   * Look up a mailing ID by job ID.
   * @param int $jobId
   *   The ID number of the job.
   *
   * @return int
   *   The ID number of the corresponding mailing
   */
  public static function mailingIdFromJob($jobId) {
    $cachedMailingIDs = Civi::cache()->get('airmailMailingIds') ?: array();
    if (empty($cachedMailingIDs[$jobId])) {
      try {
        $cachedMailingIDs[$jobId] = civicrm_api3('MailingJob', 'getvalue', array(
          'return' => "mailing_id",
          'id' => $jobId,
        ));
        Civi::cache()->set('airmailMailingIds', $cachedMailingIDs);
      }
      catch (CRM_Core_Exception $e) {
        $error = $e->getMessage();
        CRM_Core_Error::debug_log_message(self::ts('API Error retrieving mailing ID: %1', [1 => $error]));
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
    $mailingCache = Civi::cache()->get('airmailMailingCache') ?: array();
    if (empty($mailingCache[$mailingId])) {
      $mailingCache[$mailingId] = civicrm_api3('Mailing', 'getsingle', array('id' => $mailingId));
      Civi::cache()->set('airmailMailingCache', $mailingCache);
    }
    return $mailingCache[$mailingId];
  }

}
