<?php

use CRM_Airmail_Utils as E;

class CRM_Airmail_Backend_Elastic implements CRM_Airmail_Backend {

  public function processInput($input) {
    // ElasticEmail sends parameters in $_GET
    return $_GET;
  }

  public function validateMessages($event) {
    return !empty($event['transaction']);
  }

  public function processMessages($event) {
    // Keep a log to check what we're being sent.
    // file_put_contents("/path/to/somewhere/private/elasticemail-events.json", json_encode($event), FILE_APPEND);

    // Parse postback url to get all required mailing information.
    $mailingJobInfo = E::parseSourceString($event['postback']);
    $commonEventParams = [
      'job_id'         => $mailingJobInfo['job_id'],
      'event_queue_id' => $mailingJobInfo['event_queue_id'],
      'hash'           => $mailingJobInfo['hash'],
    ];

    // 2020-12-03 Status values are defined as one of:
    // Sent, Opened, Clicked, Error, AbuseReport, Unsubscribed
    // https://help.elasticemail.com/en/articles/2376855-how-to-manage-http-web-notifications-webhooks
    switch ($event['status']) {
      // When you want to receive notifications for bounced emails.
      case 'Error':
        // Map Elastic email bounce types and resoan with CiviCRM.
        $bounce_details = $this->getBounceTypeMessages($event);
        if (!$bounce_details['bounce_type_id']) {
          // No bounce type, don't record a bounce.
          // Log it though, because it's unusual.
          Civi::log()->notice('Unrecognised Elastic Email webhook event received: '
            . $bounce_details['bounce_reason']
            . "Event data: " . json_encode($event));
          return;
        }
        $commonEventParams = $commonEventParams + $bounce_details;
        CRM_Airmail_EventAction::bounce($commonEventParams);
        break;

      case 'AbuseReport':
        $commonEventParams += [
          'bounce_type_id' => 10,
          'bounce_reason'  => 'Abuse reported by user'
        ];
        CRM_Airmail_EventAction::bounce($commonEventParams);
        break;

      case 'Sent':
      case 'Opened':
      case 'Clicked':
        // We do not take any action.
        break;

      case 'Unsubscribed':
        // This will happen if the user used an Elastic Email unsubscribe link.
        //
        // Discussion:
        //
        // If they used EE's default unsubscribe link, it means Elastic will
        // never email them again, so it's like CiviCRM's optout, in which case
        // we might process like this:
        //
        // CRM_Airmail_EventAction::unsubscribe($commonEventParams + ['org_unsubscribe' => 1]);
        //
        // However, we could also get sent these events when we have wrapped
        // their unsubscribe link in ours, in which case we've already handled
        // the unsubscribe, which could have been just to a list (not a global
        // opt-out) and so doing the above would be bad.
        //
        // Therefore, for now, this does nothing.
        //
        // Ideally, things are configured so that every EE unsubscribe link wraps around
        // a CiviCRM one, in which case, ignoring this is fine.
        break;

      default:
        Civi::log()->error("Elastic Email webhook received with undocumented status: " . json_encode($event['status']));
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

    // Has Track Stats Only been negotiated?
    $settings = E::getSettings();
    $trackStatsOnly = !empty($settings['ee_wrapunsubscribe']);
    $preUnsubscribeText = E::ts(trim($settings['ee_unsubscribe'] ?? '', " \r\n"));
    $contactID = $params['contactId'] ?? $params['contactID'] ?? $params['contact_id'] ?? NULL;

    //file_put_contents("/var/www/support.opendemocracy.net/sites/default/files/private/alterMailParams.json", json_encode($params + ['trackStatsOnly' => $trackStatsOnly , 'settings' => $settings, 'contactID' => $contactID]));

    // Add custom headers for ElasticEmail
    // This is required so we will get postback URL in Elastic email response,
    // and can then identify the mailing item, source contact, etc.

    switch ($context) {
    case 'flexmailer':
    case 'civimail':
      // Bulk mail.
      //
      // Should have a Return-Path with the VERP data, copy that to the special
      // Elastic Email postback header.
      if (!empty($params['Return-Path'])) {
        $params['headers']['X-ElasticEmail-Postback'] = $params['Return-Path'];
      }

      // Elastic Email insists that we wrap our unsubscribe links so they can
      // monitor it. If we don't do this then they will add an unsubscribe
      // footer of their own which won't use CiviCRM's unsubscribe and is
      // therefore not usually what we want.
      //
      // The following tries to identify CiviCRM unsubscribe and optout links and
      // wraps them so
      // <a href="https://eg.com/civicrm/mailing/unsubscribe?x=y">unsub</a>
      // becomes
      // <a href="{unsubscribe:https://eg.com/civicrm/mailing/unsubscribe?x=y">unsub</a>}>
      //
      // This is always done for optout links (the "no bulk mail"), but is only
      // done for group-specific unsubscribe links if ee_wrapunsubscribe is
      // set.
      //
      // See docs/Elastic.md
      //

      // Nb. the patterns differ in whether it's href='foo' or href="foo"
      if ($trackStatsOnly) {
        // Wrap optout and unsubscribe.
        $patterns = [
          '@href=(")(https?://[^"]+?civicrm/mailing/(?:unsubscribe|optout)[^"]+?)(")@',
          "@href=(')(https?://[^']+?civicrm/mailing/(?:unsubscribe|optout)[^']+?)(')@",
        ];
      }
      else {
        // Only wrap optout links.
        $patterns = [
          '@href=(")(https?://[^"]+?civicrm/mailing/optout[^"]+?)(")@',
          "@href=(')(https?://[^']+?civicrm/mailing/optout[^']+?)(')@",
        ];
      }
      // Apply the patterns.
      $params['html'] = preg_replace($patterns, 'href=$1{unsubscribe:$2}$3', $params['html']);
      // @todo apply to text?

      // Deliberately no 'break': fall-through to singleEmail...

    case 'singleEmail':
      // This is the case for messageTemplate sending after tokenisation, as
      // well as other specialist single email sends.

      $deleteLink = FALSE;

      if (!empty($params['html']) && !preg_match('/{unsubscribe[:}]/', $params['html'])) {
        // The HTML content is missing an ElasticEmail unsubscribe.

        // If the HTML includes a body tag, we'll need to insert our link just
        // before the closing tag.
        $bodyEnd = strpos($params['html'], '</body>');
        if ($bodyEnd === FALSE) {
          $bodyEnd = strlen($params['html']);
        }

        if ($trackStatsOnly && $contactID) {
          $deleteLink = $this->getDeleteMyEmailLink($contactID, $params['toEmail']);
        }
        if ($deleteLink) {
          // We have been able to get our special delete URL. Wrap link
          // in Elastic Email's {unsubscribe:xxx} token:
          $content = '<a href="{unsubscribe:' . htmlspecialchars($deleteLink)
            . '}" style="color:#bbb;">'
            . E::ts('Delete my email')
            . '</a>';
        }
        else {
          // We were unable to get our special delete URL but we still have to include
          // Elastic Email's unsubscribe.
          $content = htmlspecialchars($preUnsubscribeText) . ' {unsubscribe}';
        }

        // Inject our link.
        $params['html'] = substr($params['html'], 0, $bodyEnd)
          . '<div style="font-size:12px;color:#bbb;line-height:1;padding-top:160px;background:white;">'
          . $content
          . '</div>'
          . substr($params['html'], $bodyEnd);
      }

      if (!empty($params['text']) && !preg_match('/{unsubscribe[:}]/', $params['text'])) {
        // The Text content is missing an ElasticEmail unsubscribe.
        if ($trackStatsOnly && $contactID && $deleteLink === FALSE) {
          // We have not yet looked up the deleteLink, so try now.
          // (This will be the case if the email has no HTML part.)
          $deleteLink = $this->getDeleteMyEmailLink($contactID, $params['toEmail']);
        }
        if ($deleteLink) {
          // We have been able to get our special delete URL. Wrap link
          // in Elastic Email's {unsubscribe:xxx} token:
          $content = "{unsubscribe:$deleteLink}";
        }
        else {
          $content = "$preUnsubscribeText {unsubscribe}";
        }

        // Inject the link
        $params['text'] .= "\r\n\r\n\r\n$content";
      }
      break;

    case 'messageTemplate':
      // Do nothing (Message templates call this hook twice. We catch those in
      // the 2nd phase where the context is singleEmail)
      break;

    default:
      // Make noise if we find something unexpected.
      Civi::log()->notice("Undocumented hook_civicrm_alterMailParams context value: " . json_encode($context));
    }
      // Since we're capable of and our users are data controllers responsible
      // for handling unsubscribes ourselves, we can avert EE's link additons
      // by hiding their unsubscribe link. However you must still ensure your
      // use of their service is within the T&C; they do require that every email
      // has a working unsubscribe/optout link.
      // $params['html'] .= '<!--<a href="{unsubscribe}"></a>-->';
  }

 /**
  * Translate Elastic Email event category to bounce type and message.
  *
  * @param Array $event
  *
  * @return Array with keys: bounce_type_id and bounce_reason
  */
  public function getBounceTypeMessages($event) {

    // 2020-12-03: Elastic Email's devs confirmed that the following list provides the correct reference
    // https://api.elasticemail.com/public/help#classes_MessageCategory
    // CiviCRM's types are found in the civicrm_mailing_bounce_type table.
    $mapElasticCategoryToCiviBounceType = [
      'AccountProblem'        => 8,  // Quota    3 tries
      'BlackListed'           => 10, // Spam    immediate hold
      'CodeError'             => 2,  // Away    30 tries
      'ConnectionProblem'     => 11, // Syntax   3 tries
      'ConnectionTerminated'  => 10, // Spam    immediate hold
      'DNSProblem'            => 3,  // DNS      3 tries
      'GreyListed'            => 2,  // Away    30 tries
      'Ignore'                => 8, // Although undefined, this is used for quota errors.
      'ManualCancel'          => 6,  // Invalid immediate hold
      'NoMailbox'             => 6,  // Invalid immediate hold
      'NotDelivered'          => 6,  // Invalid immediate hold
      'NotDeliveredCancelled' => 6,  // Invalid immediate hold ??? undocumented.
      'SPFProblem'            => 11, // Syntax   3 tries
      'Spam'                  => 10, // Spam    immediate hold
      'Throttled'             => 2,  // Away    30 tries
      'Timeout'               => 9,  // Relay    3 tries
      'Unknown'               => 2,  // Away    30 tries
    ];

    $category = $event['category'];
    $bounce_type_id = $mapElasticCategoryToCiviBounceType[$event['category']] ?? NULL;
    if (!$bounce_type_id) {
      $category = 'Undocumented';
    }

    // Add a description for the cause of the bounce (map from Elastic Email bounce category).
    // see https://help.elasticemail.com/en/articles/2300650-what-are-the-bounce-error-categories-and-filters
    $mapCategoryToMessage = [
      'AccountProblem'        => 'There is something wrong with the mailbox of the recipient, eg. the mailbox is full and cannot accept more emails',
      'BlackListed'           => 'Email is black listed',
      'CodeError'             => 'Error at Elastic Email; will be retried; Contact Elastic Email if a recurring problem.',
      'ConnectionProblem'     => 'The email was not delivered because of a connection problem',
      'ConnectionTerminated'  => 'The status of the email is not known for sure because the recipient server terminated the connection without returning a message code or status',
      'DNSProblem'            => 'The domain part of the address does not exist',
      'GreyListed'            => 'Greylisted, will be retried automatically: The email was not delivered because the recipient server has determined that this email has not been seen in the configuration provide',
      'Ignore'                => '"Ignore". Presumably will be retried automatically?',
      'ManualCancel'          => 'Email canceled by an Elastic Email administrator or you canceled the email yourself from the Elastic Email website',
      'NoMailbox'             => 'The email address does not appear to exist',
      'NotDelivered'          => 'The recipient has a blocked status for either hard bouncing, being unsubscribed or complained',
      'NotDeliveredCancelled' => '(undocumented) The recipient has a blocked status for either hard bouncing, being unsubscribed or complained',
      'SPFProblem'            => 'The email was not delivered because there was an issue validating the SPF record for the domain of this email',
      'Spam'                  => 'The email was rejected because it matched a profile the internet community has labeled as Spam',
      'Throttled'             => 'The recipient server did not accept the email within 48 hours',
      'Timeout'               => 'The email was not delivered because a timeout occurred',
      'Unknown'               => 'Unknown Error',
      'Undocumented'          => "Received undocumented category '$event[category]'",
    ];

    // return CiviCRM bounce code and description.
    return ['bounce_type_id' =>  $bounce_type_id, 'bounce_reason' => $mapCategoryToMessage[$category]];
  }
  /**
   * Try to generate a link to the deletemyemail form.
   *
   * @return ?string
   */
  public function getDeleteMyEmailLink($contactID, $email) {
    if (!$contactID || !$email) {
      Civi::log()->notice("Missing one of contactID or email");
      return;
    }
    $emailDao = new CRM_Core_DAO_Email();
    $emailDao->email = $email;
    $emailDao->contact_id = $contactID;
    if ($emailDao->find(TRUE)) {
      // We have all we need to generate a checksum-ed optout link.
      $query = [
        'reset' => 1,
        'cid' => $contactID,
        'eid' => $emailDao->id,
        'cs' => CRM_Contact_BAO_Contact_Utils::generateChecksum($contactID),
      ];
      // Create a raw front-end absolute URL
      Civi::log()->notice("Email found returning a link");
      return CRM_Utils_System::url('civicrm/deletemyemail', $query, TRUE, NULL, FALSE, TRUE);
    }
    else {
      Civi::log()->notice("Email not found $email for $contactID");
    }
  }
}
