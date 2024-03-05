<?php

use CRM_Airmail_Utils as E;

class CRM_Airmail_Backend_Ses implements CRM_Airmail_Backend {

  public function processInput($input) {
    return json_decode($input);
  }

  public function validateMessages($events) {
    // No validation performed at this point.
    return TRUE;
  }

  /**
   * Process Events from Amazon SNS on behalf of Amazon SES
   *
   * @param  object $events json decoded object sent from SES
   */
  public function processMessages($events) {
    switch ($events->Type) {
      case 'SubscriptionConfirmation':
        // If the message is to confirm subscription to SNS
        if (!empty($events->SubscribeURL)) {
          // Go to the subscribe URL to confirm end point
          // TODO: parse the xml and save the info to civi just in case
          file_get_contents($events->SubscribeURL);
        }
        break;

      case 'Notification':
        // If the message is a notification of a mailing event
        $responseMessage = json_decode($events->Message);

        if (!empty($responseMessage->mail->headers)) {
          foreach ($responseMessage->mail->headers as $header) {
            if ($header->name == 'X-CiviMail-Bounce') {
              $bounceAddress = $header->value;
            }
          }
        }
        if (empty($bounceAddress)) {
          $bounceAddress = $responseMessage->mail->source;
        }
        $params = E::parseSourceString($bounceAddress);

        //No point in continuing if any required bounce parameters are missing.
        if (empty($params)) {
          CRM_Core_Error::debug_log_message("Airmail error: could not parse bounce email '{$bounceAddress}' (for destination " . implode(", ", $responseMessage->mail->destination) . ").");
          return;
        }

        switch ($responseMessage->notificationType) {

          case 'Bounce':
            $params['bounce_type_id'] = self::getBounceTypeId($responseMessage->bounce->bounceType, $responseMessage->bounce->bounceSubType);
            $params['bounce_reason'] = "Bounce via SES: {$responseMessage->bounce->bounceType} {$responseMessage->bounce->bounceSubType}";
            CRM_Airmail_EventAction::bounce($params);
            break;

          case 'Complaint':
            $reasonParts = [];
            if (!empty($responseMessage->complaint->userAgent)) {
              $reasonParts[] = $responseMessage->complaint->userAgent;
            }
            if (!empty($responseMessage->complaint->complaintFeedbackType)) {
              $reasonParts[] = $responseMessage->complaint->complaintFeedbackType;
            }
            if (!empty($responseMessage->complaint->complaintSubType)) {
              $reasonParts[] = $responseMessage->complaint->complaintSubType;
            }
            if ($reasonParts) {
              $params['bounce_reason'] = "Complaint via SES: " . implode(" ", $reasonParts);
            } else {
              $params['bounce_reason'] = "Complaint via SES (no further details)";
            }
            CRM_Airmail_EventAction::bounce($params);
            // TODO: switch from bounce to complaint if/when it is implemented
            // CRM_Airmail_EventAction::complaint($params);
            break;
        }
        break;
    }
  }

  /**
   * See https://docs.aws.amazon.com/ses/latest/DeveloperGuide/notification-contents.html#bounce-types
   */
  public static function getBounceTypeId($bounceType, $bounceSubType) {
    // 6 = Invalid
    // 8 = Quota
    // 9 = Relay
    // 10 = Spam
    $sesBounceTypes['Undetermined']['Undetermined'] = 6;
    $sesBounceTypes['Permanent']['General'] = 6;
    $sesBounceTypes['Permanent']['NoEmail'] = 6;
    $sesBounceTypes['Permanent']['Suppressed'] = 6;
    $sesBounceTypes['Permanent']['OnAccountSuppressionList'] = 6;
    $sesBounceTypes['Transient']['General'] = 9;
    $sesBounceTypes['Transient']['MailboxFull'] = 8;
    $sesBounceTypes['Transient']['MessageTooLarge'] = 9;
    $sesBounceTypes['Transient']['ContentRejected'] = 10;
    $sesBounceTypes['Transient']['AttachmentRejected'] = 10;
    return $sesBounceTypes[$bounceType][$bounceSubType];
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
  }
}
