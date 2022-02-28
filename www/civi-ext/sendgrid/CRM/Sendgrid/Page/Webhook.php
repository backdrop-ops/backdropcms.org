<?php

class CRM_Sendgrid_Page_Webhook extends CRM_Core_Page {
  public function run() {
    $events = json_decode(file_get_contents('php://input'));

    $settings = CRM_Sendgrid_Utils::getSettings();

    if (!$events || !is_array($events)
      || (!empty($settings['secretcode']) && $settings['secretcode'] != CRM_Utils_Array::value('secretcode', $_REQUEST))) {
      // SendGrid sends a json encoded array of events
      // if that's not what we get, we're done here
      // or if the secret code doesn't match
      CRM_Utils_System::setHttpHeader("Status", "404 Not Found");
      CRM_Utils_System::civiExit();
    }

    $config = CRM_Core_Config::singleton();
    $delivered = array();

    foreach ($events as $event) {
      if (empty($event->job_id)) {
        // Something that wasn't sent through a CiviMail job
        continue;
      }

      switch ($event->event) {
        case 'deferred':
          // temp failure, just write it to the log
          CRM_Core_Error::debug_log_message("Sendgrid webhook (deferred)\n" . print_r($event, TRUE));
          break;

        case 'bounce':
          self::bounce($event);
          break;

        case 'spamreport':
          self::spamreport($event);
          break;

        case 'unsubscribe':
          self::unsubscribe($event);
          break;

        case 'dropped':
          // if dropped because of previous bounce, unsubscribe, or spam report, treat it as such...
          // ...otherwise log it
          if ($event->reason == 'Bounced Address') {
            self::bounce($event);
          }
          elseif ($event->reason == 'Unsubscribed Address') {
            self::unsubscribe($event);
          }
          elseif ($event->reason == 'Spam Reporting Address') {
            self::spamreport($event);
          }
          else {
            CRM_Core_Error::debug_log_message("Sendgrid webhook (dropped)\n" . print_r($event, TRUE));
          }
          break;

        case 'open':
          CRM_Mailing_Event_BAO_Opened::open($event->event_queue_id);
          break;

        case 'click':
          $mailingId = CRM_Sendgrid_Utils::mailingIdFromJob($event->job_id);
          $trackerId = CRM_Mailing_BAO_TrackableURL::getTrackerURLId($url, $mailingId);
          CRM_Mailing_Event_BAO_TrackableURLOpen::track($event->event_queue_id, $trackerId);
          break;
      }
    }

    CRM_Utils_System::civiExit();
  }

  public static function bounce($event) {
    try {
      civicrm_api3('Mailing', 'event_bounce', array(
        'job_id' => $event->job_id,
        'event_queue_id' => $event->event_queue_id,
        'hash' => $event->hash,
        'body' => $event->reason,
      ));
    }
    catch (CiviCRM_API3_Exception $e) {
      CRM_Core_Error::debug_log_message("SendGrid webhook (bounce)\n" . $e->getMessage());
    }
  }

  public static function unsubscribe($event) {
    try {
      civicrm_api3('MailingGroup', 'event_unsubscribe', array(
        'job_id' => $event->job_id,
        'event_queue_id' => $event->event_queue_id,
        'hash' => $event->hash,
      ));
    }
    catch (CiviCRM_API3_Exception $e) {
      CRM_Core_Error::debug_log_message("SendGrid webhook ($event->event)\n" . $e->getMessage());
    }
  }

  public static function spamreport($event) {
    // TODO: This needs to be replaced with something else like in
    // https://github.com/cividesk/com.cividesk.email.sparkpost/blob/master/CRM/Sparkpost/Page/callback.php#L95
    // which isn't ideal but will do the trick.
    // CRM_Mailing_Event_BAO_SpamReport::report($event->event_queue_id);
    self::unsubscribe($event);
  }

}
