<?php

use CRM_Airmail_Utils as E;

class CRM_Airmail_Backend_Sendgrid implements CRM_Airmail_Backend {

  public function processInput($input) {
    return json_decode($input, TRUE);
  }

  public function validateMessages($events) {
    return is_array($events);
  }

  public function processMessages($events) {
    foreach ($events as $event) {
      if (empty($event->civimail_source)) {
        // Something that wasn't sent through a CiviMail job
        continue;
      }

      $mailingJobInfo = E::parseSourceString($event->civimail_source);

      $params = [
        'job_id' => $mailingJobInfo['job_id'],
        'event_queue_id' => $mailingJobInfo['event_queue_id'],
        'hash' => $mailingJobInfo['hash'],
      ];

      switch ($event->event) {
        case 'deferred':
          // temp failure, just write it to the log
          Civi::log()->debug("Sendgrid webhook (deferred)\n" . print_r($event, TRUE));
          break;

        case 'blocked':
        case 'bounce':
          $params['body'] = $event->reason;
          CRM_Airmail_EventAction::bounce($params);
          break;

        case 'spamreport':
          CRM_Airmail_EventAction::spamreport($params);
          break;

        case 'unsubscribe':
          CRM_Airmail_EventAction::unsubscribe($params);
          break;

        case 'dropped':
          $reason = empty($event->reason) ? E::ts('No reason') : $event->reason;
          $params['body'] = E::ts('Dropped: %1', [1 => $reason]);
          CRM_Airmail_EventAction::bounce($params);
          break;

        case 'open':
          CRM_Airmail_EventAction::open($params);
          break;

        case 'click':
          $params['url'] = $event->url;
          CRM_Airmail_EventAction::click($params);
          break;
      }
    }
  }

  /**
   * Called by hook_civicrm_alterMailParams
   *
   * @param array $params
   *   The mailing params
   * @param string $context
   *   The mailing context.
   */
  public function alterMailParams(&$params, $context) {
    // If the context is missing or there is no return path set, do nothing.
    if (empty($params['Return-Path']) || !in_array($context, ['civimail', 'flexmailer'])) {
      return;
    }
    $header = ['unique_args' => ['civimail_source' => $params['Return-Path']]];
    $params['X-SMTPAPI'] = trim(substr(preg_replace('/(.{1,70})(,|:|\})/', '$1$2' . "\n", 'X-SMTPAPI: ' . json_encode($header)), 11));
  }

}
