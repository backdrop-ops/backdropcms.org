<?php

require_once 'sendgrid.civix.php';

/**
 * Implements hook_civicrm_alterMailParams().
 */
function sendgrid_civicrm_alterMailParams(&$params, $context) {
  // If the context is missing or there is no return path set, do nothing.
  if (empty($params['Return-Path']) || !in_array($context, ['civimail', 'flexmailer'])) {
    return;
  }

  $config = CRM_Core_Config::singleton();
  @list($ignore, $job_id, $event_queue_id, $hash) = explode($config->verpSeparator, substr($params['Return-Path'], 0, strpos($params['Return-Path'], '@')));

  if (!$job_id) {
    return;
  }

  try {
    $mailing = CRM_Sendgrid_Utils::getMailingByJob($job_id);
    $settings = CRM_Sendgrid_Utils::getSettings();
    $clicktrack = (int) ($settings['open_click_processor'] == 'SendGrid' && $mailing['url_tracking']);
    $opentrack = (int) ($settings['open_click_processor'] == 'SendGrid' && $mailing['open_tracking']);

    // prepare the SendGrid SMTP API header
    $header = array(
      'filters' => array(
        'clicktrack' => array(
          'settings' => array('enable' => $clicktrack),
        ),
        'opentrack' => array(
          'settings' => array('enable' => $opentrack),
        ),
      ),
      'unique_args' => array(
        'job_id' => $job_id,
        'event_queue_id' => $event_queue_id,
        'hash' => $hash,
      ),
    );
    $params['X-SMTPAPI'] = trim(substr(preg_replace('/(.{1,70})(,|:|\})/', '$1$2' . "\n", 'X-SMTPAPI: ' . json_encode($header)), 11));

    if ($opentrack && !empty($params['html'])) {
      // remove the CiviMail generated open tracking img
      $img = '#<img src="' . $config->userFrameworkResourceURL . "extern/open\.php\?q=$event_queue_id\".*?>#";
      $params['html'] = preg_replace($img, '', $params['html']);
    }
  }
  catch (CiviCRM_API3_Exception $e) {
    CRM_Core_Error::debug_log_message($e->getMessage() . print_r($params, true));
  }
}

/**
 * hook_civicrm_buildForm
 *
 * set tracking options for mailing
 */
function sendgrid_civicrm_buildForm($formName, &$form) {
  if (($formName == 'CRM_Mailing_Form_Settings') && ($form->elementExists('url_tracking'))) {

    $settings = CRM_Sendgrid_Utils::getSettings();
    $track = (CRM_Utils_Array::value('open_click_processor', $settings) != 'Never');
    if (!$track) {

      $toFreeze = array(
        'url_tracking',
        'open_tracking',
      );
      foreach ($toFreeze as $elName) {
        $el = $form->getElement($elName);
        $el->freeze();
      }
    }
    $form->setDefaults(array(
      'url_tracking' => $track,
      'open_tracking' => $track,
    ));
  }
}

/*
 * hook_civicrm_navigationMenu
 *
 * add "SendGrid Configuration" to the Mailings menu
 */
function sendgrid_civicrm_navigationMenu(&$menu) {

  $adder = new CRM_Sendgrid_NavAdd($menu);

  $attributes = array(
    'label' => ts('SendGrid Configuration'),
    'name' => 'SendGrid Configuration',
    'url' => 'civicrm/sendgrid',
    'permission' => 'access CiviMail,administer CiviCRM',
    'operator' => 'AND',
    'separator' => 1,
    'active' => 1,
  );
  $adder->addItem($attributes, array('Mailings'));
  $menu = $adder->getMenu();
}

// *************************************
// THE REST IS JUST STANDARD BOILERPLATE
// *************************************

/**
 * Implementation of hook_civicrm_config
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_config
 */
function sendgrid_civicrm_config(&$config) {
  _sendgrid_civix_civicrm_config($config);
}

/**
 * Implementation of hook_civicrm_xmlMenu
 *
 * @param $files array(string)
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_xmlMenu
 */
function sendgrid_civicrm_xmlMenu(&$files) {
  _sendgrid_civix_civicrm_xmlMenu($files);
}

/**
 * Implementation of hook_civicrm_enable
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_enable
 */
function sendgrid_civicrm_enable() {
  _sendgrid_civix_civicrm_enable();
}

/**
 * Implementation of hook_civicrm_disable
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_disable
 */
function sendgrid_civicrm_disable() {
  _sendgrid_civix_civicrm_disable();
}

/**
 * Implementation of hook_civicrm_upgrade
 *
 * @param $op string, the type of operation being performed; 'check' or 'enqueue'
 * @param $queue CRM_Queue_Queue, (for 'enqueue') the modifiable list of pending up upgrade tasks
 *
 * @return mixed  based on op. for 'check', returns array(boolean) (TRUE if upgrades are pending)
 *                for 'enqueue', returns void
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_upgrade
 */
function sendgrid_civicrm_upgrade($op, CRM_Queue_Queue $queue = NULL) {
  return _sendgrid_civix_civicrm_upgrade($op, $queue);
}

/**
 * Implementation of hook_civicrm_managed
 *
 * Generate a list of entities to create/deactivate/delete when this module
 * is installed, disabled, uninstalled.
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_managed
 */
function sendgrid_civicrm_managed(&$entities) {
  _sendgrid_civix_civicrm_managed($entities);
}

/**
 * Implementation of hook_civicrm_caseTypes
 *
 * Generate a list of case-types
 *
 * Note: This hook only runs in CiviCRM 4.4+.
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_caseTypes
 */
function sendgrid_civicrm_caseTypes(&$caseTypes) {
  _sendgrid_civix_civicrm_caseTypes($caseTypes);
}

/**
 * Implementation of hook_civicrm_alterSettingsFolders
 *
 * @link http://wiki.civicrm.org/confluence/display/CRMDOC/hook_civicrm_alterSettingsFolders
 */
function sendgrid_civicrm_alterSettingsFolders(&$metaDataFolders = NULL) {
  _sendgrid_civix_civicrm_alterSettingsFolders($metaDataFolders);
}
