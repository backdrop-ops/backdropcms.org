<?php

require_once 'airmail.civix.php';
use CRM_Airmail_Utils as E;

/**
 * Implements hook_civicrm_alterMailParams().
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_alterMailParams
 */
function airmail_civicrm_alterMailParams(&$params, $context) {
  $backend = E::getBackend();
  if (!$backend || !in_array('CRM_Airmail_Backend', class_implements($backend))) {
    return;
  }
  $backend->alterMailParams($params, $context);
  // Create meta data for transactional email
  if ($context != 'civimail' && $context != 'flexmailer') {
    $mail = new CRM_Mailing_DAO_Mailing();
    $mail->name = 'Airmail Transactional Emails';

    if ($mail->find(TRUE)) {
      if (!empty($params['contact_id'])) {
        $contactId = $params['contact_id'];
      }
      elseif (!empty($params['contactId'])) {
        // Contribution/Event confirmation
        $contactId = $params['contactId'];
      }
      else {
        // As last option from emall address
        $contactId = airmail_targetContactId($params['toEmail']);
      }

      if (!$contactId) {
        // Not particularly useful, but devs can insert a backtrace here if they want to debug the cause.
        // Example: for context = singleEmail, we end up here. We should probably fix core.
        Civi::log()->warning('ContactId not known to attach header for this transactional email by Airmail extension possible duplicates email hence skipping: ' . CRM_Utils_Array::value('toEmail', $params));
        return;
      }

      if ($contactId) {
        $eventQueue = CRM_Mailing_Event_BAO_Queue::create([
          'job_id' => CRM_Core_DAO::getFieldValue('CRM_Mailing_DAO_MailingJob', $mail->id, 'id', 'mailing_id'),
          'contact_id' => $contactId,
          'email_id' => CRM_Core_DAO::getFieldValue('CRM_Core_DAO_Email', $contactId, 'id', 'contact_id'),
          'mailing_id' => $mail->id,
        ]);

        // Add m to differentiate from bulk mailing
        $emailDomain = CRM_Core_BAO_MailSettings::defaultDomain();
        $verpSeparator = CRM_Core_Config::singleton()->verpSeparator;
        $params['returnPath'] = implode($verpSeparator, ['m', $eventQueue->job_id, $eventQueue->id, $eventQueue->hash]) . "@$emailDomain";

        // add custom headers
        $params['headers']['X-My-Header'] = "This is the mail system at host " . "@$emailDomain" . "I am sorry to have to inform you that your message could not be delivered to one or more recipients. It is attached below. ";

        // add a tracking img if enabled.
        if ($mail->open_tracking && !empty($params['html'])) {
          $params['html'] .= "\n" . '<img src="' . CRM_Utils_System::externUrl('extern/open', "q=$eventQueue->id")
            . '" width="1" height="1" alt="" border="0">';
        }
      }
    }
    else {
      Civi::log()->debug('Airmail: the mailing for transactional emails was not found. Bounces will not be tracked. Disable/enable the Airmail extension to re-create the mailing.');
    }
  }
}

/**
 * Returns the contact_id for a specific email address.
 * Returns NULL if no contact was found, or if more than one
 * contact was matched.
 *
 * @return contact Id | NULL
 */
function airmail_targetContactId($email) {
  // @todo Does this exclude deleted contacts?
  $result = civicrm_api3('email', 'get', [
    'email' => trim($email),
    'sequential' => 1,
  ]);

  if ($result['count'] == 1) {
    return $result['values'][0]['contact_id'];
  }

  return NULL;
}

/**
 * hook_civicrm_navigationMenu
 *
 * add "Airmail Configuration" to the Mailings menu
 */
function airmail_civicrm_navigationMenu(&$menu) {

  $adder = new CRM_Airmail_NavAdd($menu);

  $attributes = array(
    'label' => E::ts('Airmail Configuration'),
    'name' => 'Airmail Configuration',
    'url' => 'civicrm/airmail/settings',
    'permission' => 'access CiviMail,administer CiviCRM',
    'operator' => 'AND',
    'separator' => 1,
    'active' => 1,
  );
  $adder->addItem($attributes, array('Mailings'));
  $menu = $adder->getMenu();
}

/**
 * Implements hook_civicrm_config().
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_config
 */
function airmail_civicrm_config(&$config) {
  _airmail_civix_civicrm_config($config);
}

/**
 * Implements hook_civicrm_install().
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_install
 */
function airmail_civicrm_install() {
  _airmail_civix_civicrm_install();
}

/**
 * Implements hook_civicrm_enable().
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_enable
 */
function airmail_civicrm_enable() {
  _airmail_civix_civicrm_enable();
}

/**
 * Implements hook_civicrm_managed().
 *
 * Generate a list of entities to create/deactivate/delete when this module
 * is installed, disabled, uninstalled.
 *
 * @link https://docs.civicrm.org/dev/en/latest/hooks/hook_civicrm_managed
 */
function airmail_civicrm_managed(&$entities) {
  CRM_Airmail::createTransactionalMailing();
}
