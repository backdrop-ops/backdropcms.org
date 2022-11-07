<?php
/**
 * @file
 * The code processing mail in the smtp module.
 *
 */

/**
 * Modify the Backdrop mail system to use SMTP when sending emails.
 * Include the option to choose between plain text or HTML.
 */
class SmtpOAuthMailSystem extends SmtpMailSystem {

  /**
   * Google Client.
   *
   * @var \Google_Client
   */
  protected $googleClient;

  /**
   * Constructs a SmtpOAuthMailSystem object.
   */
  public function __construct() {
    $gauth_account_id =config_get('smtp_oauth.settings', 'smtp_oauth_google_api_client_id');
    $this->googleClient = gauth_client_get($gauth_account_id, FALSE);
  }

  /**
   * Send the e-mail message.
   *
   * @param array $message
   *   A message array, as described in hook_mail_alter().
   *
   * @return mixed
   *   TRUE if the mail was successfully accepted, otherwise FALSE.
   *
   * @throws phpmailerException.
   *
   * @see backdrop_mail()
   */
  public function mail(array $message) {
    if (config_get('smtp.settings', 'smtp_queue', FALSE)
       && (!isset($message['params']['skip_queue']) || !$message['params']['skip_queue'])) {
      smtp_send_queue($message);
      if (config_get('smtp.settings', 'smtp_debugging', SMTP_LOGGING_ERRORS) == SMTP_LOGGING_ALL) {
        watchdog('smtp', 'Queue sending mail to: @to', array('@to' => $message['to']));
      }
      return TRUE;
    }
    else {
      return $this->mailWithoutQueue($message);
    }
  }

  public function mailWithoutQueue(array $message) {
    $to = $message['to'];
    $from = $message['from'];
    $from_name = isset($message['from_name']) ? $message['from_name'] : FALSE;
    $body = $message['body'];
    $headers = $message['headers'];
    $subject = $message['subject'];

    // Optionally reroute all emails to a single address.
    $reroute_address = config_get('smtp.settings', 'smtp_reroute_address');
    if (!empty($reroute_address)) {
      $to = $reroute_address;
      // Remove any CC and BCC headers that might have been set.
      unset($headers['cc']);
      unset($headers['bcc']);
    }

    $email = config_get('smtp_oauth.settings', 'smtp_oauth_email_address');
    // Create a new PHPMailer object - autoloaded from registry.
    $mailer = new PHPMailerOAuth($email, $this->googleClient);
    //$mailer->Timeout = $this->smtpConfig->get('smtp_timeout');
    $logging = config_get('smtp.settings', 'smtp_debugging');

    // Turn on debugging, if requested.
    if ($logging == SMTP_LOGGING_ALL && user_access('administer smtp module')) {
      $mailer->SMTPDebug = TRUE;
    }

    // Set the from name. First we try to get the name from i18n, in the case
    // that it has been translated. The name is set according to the language
    // of the email being sent.
    if (empty($from_name)) {
      if (function_exists('i18n_variable_get')) {
        // The 'language' value may be stored as an object.
        $langcode = $message['language'];
        if (is_object($langcode)) {
          $langcode = $langcode->language;
        }
        if (i18n_variable_get('smtp_fromname', $langcode, '') != '') {
          $from_name = i18n_variable_get('smtp_fromname', $langcode, '');
        }
        // If value is not defined in settings, use site_name.
        else {
          $from_name = i18n_variable_get('site_name', $langcode, '');
        }
      }
      // If the i18n module is not enabled, get the From name as a
      // normal variable.
      else {
        if (config_get('smtp.settings', 'smtp_fromname') != '') {
          $from_name = config_get('smtp.settings', 'smtp_fromname');
        }
        // If value is not defined in settings, use site_name.
        else {
          $from_name = config_get('smtp.settings', 'site_name');
        }
      }
    }

    if (config_get('smtp.settings', 'smtp_client_hostname') != '') {
      $mailer->Hostname = config_get('smtp.settings', 'smtp_client_hostname');
    }

    if (config_get('smtp.settings', 'smtp_client_helo') != '') {
      $mailer->Helo = config_get('smtp.settings', 'smtp_client_helo');
    }

    //Hack to fix reply-to issue.
    if (!isset($headers['Reply-To']) || empty($headers['Reply-To'])) {
      if (strpos($from, '<')) {
        $reply = preg_replace('/>.*/', '', preg_replace('/.*</', '', $from));
      }
      else {
        $reply = $from;
      }
      $headers['Reply-To'] = $reply;
    }
    $properfrom = config_get('smtp.settings', 'smtp_from');
    if (!empty($properfrom)) {
      $headers['From'] = $properfrom;
      $from = $properfrom;
    }

    if ($from == NULL || $from == '') {
      // If from e-mail address is blank, use smtp_from config option.
      if (($from = config_get('smtp.settings', 'smtp_from')) == '') {
        // If smtp_from config option is blank, use site_email.
        if (($from = config_get('smtp.settings', 'site_mail')) == '') {
          backdrop_set_message(t('There is no submitted from address.'), 'error');
          if ($logging) {
            watchdog('smtp', 'There is no submitted from address.', array(), WATCHDOG_ERROR);
          }
          return FALSE;
        }
      }
    }
    $from_comp = $this->_get_components($from);

    if (!valid_email_address($from_comp['email'])) {
      backdrop_set_message(t('The submitted from address (@from) is not valid.', array('@from' => $from_comp['email'])), 'error');
      if ($logging) {
        watchdog('smtp', 'The submitted from address (@from) is not valid.', array('@from' => $from_comp['email']), WATCHDOG_ERROR);
      }
      return FALSE;
    }

    // Defines the From value to what we expect.
    $mailer->From     = $from_comp['email'];
    $mailer->FromName = empty($from_comp['name']) ? $from_name : $from_comp['name'];
    $mailer->Sender   = $from_comp['email'];

    // Create the list of 'To:' recipients.
    $torecipients = explode(',', $to);
    foreach ($torecipients as $torecipient) {
      $to_comp = $this->_get_components($torecipient);
      $mailer->AddAddress($to_comp['email'], $to_comp['name']);
    }

    // Parse the headers of the message and set the PHPMailer object's settings
    // accordingly.
    foreach ($headers as $key => $value) {
      //watchdog('error', 'Key: ' . $key . ' Value: ' . $value);
      switch (backdrop_strtolower($key)) {
        case 'from':
          if ($from == NULL or $from == '') {
            // If a from value was already given, then set based on header.
            // Should be the most common situation since backdrop_mail moves the
            // from to headers.
            $from           = $value;
            $mailer->From     = $value;
            // then from can be out of sync with from_name !
            $mailer->FromName = '';
            $mailer->Sender   = $value;
          }
          break;
        case 'content-type':
          // Parse several values on the Content-type header, storing them in an array like
          // key=value -> $vars['key']='value'
          $vars = explode(';', $value);
          foreach ($vars as $i => $var) {
            if ($cut = strpos($var, '=')) {
              $new_var = trim(backdrop_strtolower(backdrop_substr($var, $cut + 1)));
              $new_key = trim(backdrop_substr($var, 0, $cut));
              unset($vars[$i]);
              $vars[$new_key] = $new_var;
            }
          }
          // Set the charset based on the provided value, otherwise set it to UTF-8 (which is Backdrop's internal default).
          $mailer->CharSet = isset($vars['charset']) ? $vars['charset'] : 'UTF-8';
          // If $vars is empty then set an empty value at index 0 to avoid a PHP warning in the next statement
          $vars[0] = isset($vars[0])?$vars[0]:'';

          switch ($vars[0]) {
            case 'text/plain':
              // The message includes only a plain text part.
              $mailer->IsHTML(FALSE);
              $content_type = 'text/plain';
              break;
            case 'text/html':
              // The message includes only an HTML part.
              $mailer->IsHTML(TRUE);
              $content_type = 'text/html';
              break;
            case 'multipart/related':
              // Get the boundary ID from the Content-Type header.
              $boundary = $this->_get_substring($value, 'boundary', '"', '"');

              // The message includes an HTML part w/inline attachments.
              $mailer->ContentType = $content_type = 'multipart/related; boundary="' . $boundary . '"';
            break;
            case 'multipart/alternative':
              // The message includes both a plain text and an HTML part.
              $mailer->ContentType = $content_type = 'multipart/alternative';

              // Get the boundary ID from the Content-Type header.
              $boundary = $this->_get_substring($value, 'boundary', '"', '"');
            break;
            case 'multipart/mixed':
              // The message includes one or more attachments.
              $mailer->ContentType = $content_type = 'multipart/mixed';

              // Get the boundary ID from the Content-Type header.
              $boundary = $this->_get_substring($value, 'boundary', '"', '"');
            break;
            default:
              // Everything else is unsuppored by PHPMailer.
              backdrop_set_message(t('The %header of your message is not supported by PHPMailer and will be sent as text/plain instead.', array('%header' => "Content-Type: $value")), 'error');
              if ($logging) {
                watchdog('smtp', 'The %header of your message is not supported by PHPMailer and will be sent as text/plain instead.', array('%header' => "Content-Type: $value"), WATCHDOG_ERROR);
              }
              // Force the Content-Type to be text/plain.
              $mailer->IsHTML(FALSE);
              $content_type = 'text/plain';
          }
          break;

        case 'reply-to':
          $replyto_comp = $this->_get_components($value);
          $mailer->AddReplyTo($replyto_comp['email'], $replyto_comp['name']);
          break;

        case 'content-transfer-encoding':
          $mailer->Encoding = $value;
          break;

        case 'return-path':
          $returnpath_comp = $this->_get_components($value);
          $mailer->Sender = $returnpath_comp['email'];
          break;

        case 'mime-version':
        case 'x-mailer':
          // Let PHPMailer specify these.
          break;

        case 'errors-to':
          $mailer->AddCustomHeader('Errors-To: ' . $value);
          break;

        case 'cc':
          $ccrecipients = explode(',', $value);
          foreach ($ccrecipients as $ccrecipient) {
            $cc_comp = $this->_get_components($ccrecipient);
            $mailer->AddCC($cc_comp['email'], $cc_comp['name']);
          }
          break;

        case 'bcc':
          $bccrecipients = explode(',', $value);
          foreach ($bccrecipients as $bccrecipient) {
            $bcc_comp = $this->_get_components($bccrecipient);
            $mailer->AddBCC($bcc_comp['email'], $bcc_comp['name']);
          }
          break;

        case 'message-id':
          $mailer->MessageID = $value;
          break;

        default:
          // The header key is not special - add it as is.
          $mailer->AddCustomHeader($key . ': ' . $value);
      }
    }

    /**
     * TODO
     * Need to figure out the following.
     *
     * Add one last header item, but not if it has already been added.
     * $errors_to = FALSE;
     * foreach ($mailer->CustomHeader as $custom_header) {
     *   if ($custom_header[0] = '') {
     *     $errors_to = TRUE;
     *   }
     * }
     * if ($errors_to) {
     *   $mailer->AddCustomHeader('Errors-To: '. $from);
     * }
     */
    // Add the message's subject.
    $mailer->Subject = $subject;

    // Processes the message's body.
    switch ($content_type) {
      case 'multipart/related':
        $mailer->Body = $body;
        // TODO: Figure out if there is anything more to handling this type.
        break;

      case 'multipart/alternative':
        // Split the body based on the boundary ID.
        $body_parts = $this->_boundary_split($body, $boundary);
        foreach ($body_parts as $body_part) {
          // If plain/text within the body part, add it to $mailer->AltBody.
          if (strpos($body_part, 'text/plain')) {
            // Clean up the text.
            $body_part = trim($this->_remove_headers(trim($body_part)));
            // Include it as part of the mail object.
            $mailer->AltBody = $body_part;
          }
          // If plain/html within the body part, add it to $mailer->Body.
          elseif (strpos($body_part, 'text/html')) {
            // Clean up the text.
            $body_part = trim($this->_remove_headers(trim($body_part)));
            // Include it as part of the mail object.
            $mailer->Body = $body_part;
          }
        }
        break;

      case 'multipart/mixed':
        // Split the body based on the boundary ID.
        $body_parts = $this->_boundary_split($body, $boundary);

        // Determine if there is an HTML part for when adding the plain text part.
        $text_plain = FALSE;
        $text_html  = FALSE;
        foreach ($body_parts as $body_part) {
          if (strpos($body_part, 'text/plain')) {
            $text_plain = TRUE;
          }
          if (strpos($body_part, 'text/html')) {
            $text_html = TRUE;
          }
        }

        foreach ($body_parts as $body_part) {
          // If test/plain within the body part, add it to either
          // $mailer->AltBody or $mailer->Body, depending on whether there is
          // also a text/html part ot not.
          if (strpos($body_part, 'multipart/alternative')) {
            // Get boundary ID from the Content-Type header.
            $boundary2 = $this->_get_substring($body_part, 'boundary', '"', '"');
            // Clean up the text.
            $body_part = trim($this->_remove_headers(trim($body_part)));
            // Split the body based on the boundary ID.
            $body_parts2 = $this->_boundary_split($body_part, $boundary2);

            foreach ($body_parts2 as $body_part2) {
              // If plain/text within the body part, add it to $mailer->AltBody.
              if (strpos($body_part2, 'text/plain')) {
                // Clean up the text.
                $body_part2 = trim($this->_remove_headers(trim($body_part2)));
                // Include it as part of the mail object.
                $mailer->AltBody = $body_part2;
                $mailer->ContentType = 'multipart/mixed';
              }
              // If plain/html within the body part, add it to $mailer->Body.
              elseif (strpos($body_part2, 'text/html')) {
                // Get the encoding.
                $body_part2_encoding = trim($this->_get_substring($body_part2, 'Content-Transfer-Encoding', ':', "\n"));
                // Clean up the text.
                $body_part2 = trim($this->_remove_headers(trim($body_part2)));
                // Check whether the encoding is base64, and if so, decode it.
                if (backdrop_strtolower($body_part2_encoding) == 'base64') {
                  // Include it as part of the mail object.
                  $mailer->Body = base64_decode($body_part2);
                  // Ensure the whole message is recoded in the base64 format.
                  $mailer->Encoding = 'base64';
                }
                else {
                  // Include it as part of the mail object.
                  $mailer->Body = $body_part2;
                }
                $mailer->ContentType = 'multipart/mixed';
              }
            }
          }
          // If text/plain within the body part, add it to $mailer->Body.
          elseif (strpos($body_part, 'text/plain')) {
            // Clean up the text.
            $body_part = trim($this->_remove_headers(trim($body_part)));

            if ($text_html) {
              $mailer->AltBody = $body_part;
              $mailer->IsHTML(TRUE);
              $mailer->ContentType = 'multipart/mixed';
            }
            else {
              $mailer->Body = $body_part;
              $mailer->IsHTML(FALSE);
              $mailer->ContentType = 'multipart/mixed';
            }
          }
          // If text/html within the body part, add it to $mailer->Body.
          elseif (strpos($body_part, 'text/html')) {
            // Clean up the text.
            $body_part = trim($this->_remove_headers(trim($body_part)));
            // Include it as part of the mail object.
            $mailer->Body = $body_part;
            $mailer->IsHTML(TRUE);
            $mailer->ContentType = 'multipart/mixed';
          }
          // Add the attachment.
          elseif (strpos($body_part, 'Content-Disposition: attachment;') && !isset($message['params']['attachments'])) {
            $file_path     = $this->_get_substring($body_part, 'filename=', '"', '"');
            $file_name     = $this->_get_substring($body_part, ' name=', '"', '"');
            $file_encoding = $this->_get_substring($body_part, 'Content-Transfer-Encoding', ' ', "\n");
            $file_type     = $this->_get_substring($body_part, 'Content-Type', ' ', ';');

            if (file_exists($file_path)) {
              if (!$mailer->AddAttachment($file_path, $file_name, $file_encoding, $file_type)) {
                backdrop_set_message(t('Attahment could not be found or accessed.'));
              }
            }
            else {
              // Clean up the text.
              $body_part = trim($this->_remove_headers(trim($body_part)));

              if (backdrop_strtolower($file_encoding) == 'base64') {
                $attachment = base64_decode($body_part);
              }
              elseif (backdrop_strtolower($file_encoding) == 'quoted-printable') {
                $attachment = quoted_printable_decode($body_part);
              }
              else {
                $attachment = $body_part;
              }

              $attachment_new_filename = backdrop_tempnam('temporary://', 'smtp');
              $file_path = file_save_data($attachment, $attachment_new_filename, FILE_EXISTS_REPLACE);
              $real_path = backdrop_realpath($file_path->uri);

              if (!$mailer->AddAttachment($real_path, $file_name)) {
                backdrop_set_message(t('Attachment could not be found or accessed.'));
              }
            }
          }
        }
        break;

      default:
        $mailer->Body = $body;
        break;
    }

    // Process mimemail attachments, which are prepared in mimemail_mail().
    if (isset($message['params']['attachments'])) {
      foreach ($message['params']['attachments'] as $attachment) {
        if (isset($attachment['filecontent'])) {
          $mailer->AddStringAttachment($attachment['filecontent'], $attachment['filename'], 'base64', $attachment['filemime']);
        }
        if (!isset($attachment['filepath']) && isset($attachment['uri'])) {
          $attachment['filepath'] = $attachment['uri'];
        }
        if (isset($attachment['filepath'])) {
          $filename = isset($attachment['filename']) ? $attachment['filename'] : basename($attachment['filepath']);
          $filemime = isset($attachment['filemime']) ? $attachment['filemime'] : file_get_mimetype($attachment['filepath']);
          $mailer->AddAttachment($attachment['filepath'], $filename, 'base64', $filemime);
        }
      }
    }

    // Process inline images.
    if (!empty($message['params']['images'])) {
      foreach ($message['params']['images'] as $image) {
        if (!empty($image['filepath']) && !empty($image['cid'])) {
          if (file_exists($image['filepath'])) {
            $image_extension = pathinfo($image['filepath'], PATHINFO_EXTENSION);
            $image_mime_type = PHPMailer::_mime_types($image_extension);
            if (empty($image['name'])) {
              $image_name = pathinfo($image['filepath'], PATHINFO_FILENAME);
            }
            else {
              $image_name = $image['name'];
            }
            $mailer->AddEmbeddedImage($image['filepath'], $image['cid'], $image_name, 'base64', $image_mime_type);
          }
        }
      }
    }

    // Set the authentication settings.
    $email = config_get('smtp_oauth.settings', 'smtp_oauth_email_address');

    //Pass the OAuth provider instance to PHPMailer
    $mailer->setOAuth(
      new GmailXOAuth2($email, $this->googleClient)
    );

    $mailer->SMTPAuth = TRUE;
    //Set AuthType to use XOAUTH2
    $mailer->AuthType = 'XOAUTH2';

    // Set the protocol prefix for the smtp host.
    switch (config_get('smtp.settings', 'smtp_protocol')) {
      case 'ssl':
        $mailer->SMTPSecure = 'ssl';
        break;

      case 'tls':
        $mailer->SMTPSecure = 'tls';
        break;

      default:
        $mailer->SMTPSecure = '';
    }

    // Set SSL stream context options
    if (config_get('smtp.settings', 'smtp_protocol') != 'standard') {
      $smtp_allow_self_signed = array('allow_self_signed' => config_get('smtp.settings', 'smtp_allow_self_signed'));

      if (version_compare(phpversion(), '5.6.0', '>=')) {
        $smtp_verify_peer = array('verify_peer' => config_get('smtp.settings', 'smtp_verify_peer'));
        $smtp_verify_peer_name = array('verify_peer_name' => config_get('smtp.settings', 'smtp_verify_peer_name'));
        $mailer->SMTPOptions = array(
          'ssl' => array_merge($smtp_verify_peer, $smtp_verify_peer_name, $smtp_allow_self_signed),
        );
      }
      else {
        $smtp_verify_peer = array('verify_peer' => config_get('smtp.settings', 'smtp_verify_peer'));
        $mailer->SMTPOptions = array(
          'ssl' => array_merge($smtp_verify_peer, $smtp_allow_self_signed),
        );
      }
    }

    // Set other connection settings.
    $mailer->Host = config_get('smtp.settings', 'smtp_host') . ';' . config_get('smtp.settings', 'smtp_hostbackup');
    $mailer->Port = config_get('smtp.settings', 'smtp_port');
    $mailer->Mailer = 'smtp';

    // Integration with the Maillog module.
    if (module_exists('maillog')) {
      if (config_get('maillog.settings', 'maillog_log')) {
        $record = new stdClass;

        // In case the subject/from/to is already encoded, decode with
        // mime_header_decode.
        $record->header_message_id = isset($mailer->MessageID) ? $mailer->MessageID : NULL;
        $record->subject = backdrop_substr(mime_header_decode($mailer->Subject), 0, 255);
        $record->header_from = $from;
        $record->header_to = $to;
        $record->header_reply_to = isset($headers['Reply-To']) ? $headers['Reply-To'] : '';
        $record->header_all = serialize($headers);
        $record->sent_date = REQUEST_TIME;

        // Used to separate different portions of the body string.
        $divider = str_repeat('-', 60) . "\n";

        // Load the attachments.
        $attachments = $mailer->GetAttachments();

        $record->body = '';

        // If there's more than one item to display, add a divider.
        if (!empty($mailer->AltBody) || !empty($attachments)) {
          $record->body .= t('Body') . ":\n";
          $record->body .= $divider;
        }

        // Add the body field.
        if (isset($mailer->Body)) {
          $record->body .= $mailer->Body;
        }
        else {
          $record->body .= t('*No message body*') . ":\n";
        }

        // The AltBody value is optional.
        if (!empty($mailer->AltBody)) {
          $record->body .= "\n";
          $record->body .= $divider;
          $record->body .= t('Alternative body') . ":\n";
          $record->body .= $divider;
          $record->body .= $mailer->AltBody;
        }

        // List the attachments.
        if (!empty($attachments)) {
          $record->body .= "\n";
          $record->body .= $divider;
          $record->body .= t('Attachments') . ":\n";
          $record->body .= $divider;
          foreach ($attachments as $file) {
            $record->body .= t('Filename') . ':' . $file[1] . "\n";
            $record->body .= t('Name') . ':' . $file[2] . "\n";
            $record->body .= t('Encoding') . ':' . $file[3] . "\n";
            $record->body .= t('Type') . ':' . $file[4] . "\n";
            $record->body .= "\n";
          }
        }

        backdrop_write_record('maillog', $record);
      }

      // Display the e-mail using Devel module.
      if (config_get('maillog.settings', 'maillog_devel') && function_exists('dpm')) {
        $devel_msg = array();
        $devel_msg[t('Subject')] = $mailer->Subject;
        $devel_msg[t('From')] = $from;
        $devel_msg[t('To')] = $to;
        $devel_msg[t('Reply-To')] = isset($headers['Reply-To']) ? $headers['Reply-To'] : NULL;
        $devel_msg[t('Headers')] = $headers;
        $devel_msg[t('Body')] = $mailer->Body;
        $devel_msg[t('Alternative body')] = $mailer->AltBody;
        $devel_msg[t('Attachments')] = $mailer->GetAttachments();

        dpm($devel_msg, 'maillog');
      }
    }

    $error = FALSE;

    // Email delivery was disabled.
    if (!config_get('smtp.settings', 'smtp_deliver')) {
      if ($logging) {
        $params = array(
          '@from' => $from,
          '@to' => $to,
        );
        watchdog('smtp', 'Email delivery is disabled, did not send email from @from to @to.', $params);
      }
    }
    else {
      if (!$mailer->send()) {
        $params = array(
          '@from' => $from,
          '@to' => $to,
          '!error_message' => $mailer->ErrorInfo
        );

        if (config_get('smtp.settings', 'smtp_queue_fail')) {
          if ($logging) {
            watchdog('smtp', 'Error sending e-mail from @from to @to, will retry on cron run : !error_message.', $params, WATCHDOG_ERROR);
          }
          smtp_failed_messages($message);
        }
        elseif ($logging) {
          $error = TRUE;
          watchdog('smtp', 'Error sending e-mail from @from to @to : !error_message', $params, WATCHDOG_ERROR);
        }
      }
      elseif (config_get('smtp.settings', 'smtp_debugging') == SMTP_LOGGING_ALL) {
        watchdog('smtp', 'Sent mail to: @to', array('@to' => $to));
      }
    }

    $mailer->SmtpClose();
    return !$error;
  }

  /**
   * Splits the input into parts based on the given boundary.
   *
   * Swiped from Mail::MimeDecode, with modifications based on Backdrop's coding
   * standards and this bug report: http://pear.php.net/bugs/bug.php?id=6495
   *
   * @param input
   *   A string containing the body text to parse.
   * @param boundary
   *   A string with the boundary string to parse on.
   * @return
   *   An array containing the resulting mime parts
   */
  protected function _boundary_split($input, $boundary) {
    $parts       = array();
    $bs_possible = backdrop_substr($boundary, 2, -2);
    $bs_check    = '\"' . $bs_possible . '\"';

    if ($boundary == $bs_check) {
      $boundary = $bs_possible;
    }

    $tmp = explode('--' . $boundary, $input);

    for ($i = 1; $i < count($tmp); $i++) {
      if (trim($tmp[$i])) {
        $parts[] = $tmp[$i];
      }
    }

    return $parts;
  }  //  End of _smtp_boundary_split().

  /**
   * Strips the headers from the body part.
   *
   * @param input
   *   A string containing the body part to strip.
   * @return
   *   A string with the stripped body part.
   */
  protected function _remove_headers($input) {
    $part_array = explode("\n", $input);

    // will strip these headers according to RFC2045
    $headers_to_strip = array( 'Content-Type', 'Content-Transfer-Encoding', 'Content-ID', 'Content-Disposition');
    $pattern = '/^(' . implode('|', $headers_to_strip) . '):/';

    while (count($part_array) > 0) {

      // ignore trailing spaces/newlines
      $line = rtrim($part_array[0]);

      // if the line starts with a known header string
      if (preg_match($pattern, $line)) {
        $line = rtrim(array_shift($part_array));
        // remove line containing matched header.

        // if line ends in a ';' and the next line starts with four spaces, it's a continuation
        // of the header split onto the next line. Continue removing lines while we have this condition.
        while (substr($line, -1) == ';' && count($part_array) > 0 && substr($part_array[0], 0, 4) == '    ') {
          $line = rtrim(array_shift($part_array));
        }
      }
      else {
        // no match header, must be past headers; stop searching.
        break;
      }
    }

    $output = implode("\n", $part_array);
    return $output;
  }  //  End of _smtp_remove_headers().

  /**
   * Returns a string that is contained within another string.
   *
   * Returns the string from within $source that is some where after $target
   * and is between $beginning_character and $ending_character.
   *
   * @param $source
   *   A string containing the text to look through.
   * @param $target
   *   A string containing the text in $source to start looking from.
   * @param $beginning_character
   *   A string containing the character just before the sought after text.
   * @param $ending_character
   *   A string containing the character just after the sought after text.
   * @return
   *   A string with the text found between the $beginning_character and the
   *   $ending_character.
   */
  protected function _get_substring($source, $target, $beginning_character, $ending_character) {
    $search_start     = strpos($source, $target) + 1;
    $first_character  = strpos($source, $beginning_character, $search_start) + 1;
    $second_character = strpos($source, $ending_character, $first_character) + 1;
    $substring        = backdrop_substr($source, $first_character, $second_character - $first_character);
    $string_length    = backdrop_strlen($substring) - 1;

    if ($substring[$string_length] == $ending_character) {
      $substring = backdrop_substr($substring, 0, $string_length);
    }

    return trim($substring);
  }  //  End of _smtp_get_substring().

  /**
   * Returns an array of name and email address from a string.
   *
   * @param $input
   *  A string that contains different possible combinations of names and
   *  email address.
   * @return
   *  An array containing a name and an email address.
   */
  protected function _get_components($input) {
    $input = trim($input);
    $components = array(
      'input' => $input,
      'name' => '',
      'email' => '',
    );

    // If the input is a valid email address in its entirety, then there is
    // nothing to do, just return that.
    if (valid_email_address($input)) {
      $components['email'] = $input;
      return $components;
    }

    // Check if $input has one of the following formats, extract what we can:
    //  some name <address@example.com>
    //  "another name" <address@example.com>
    //  <address@example.com>
    if (preg_match('/^"?([^"\t\n]*)"?\s*<([^>\t\n]*)>$/', $input, $matches)) {
      $components['name'] = trim($matches[1]);
      $components['email'] = trim($matches[2]);
    }

    return $components;
  }
}
