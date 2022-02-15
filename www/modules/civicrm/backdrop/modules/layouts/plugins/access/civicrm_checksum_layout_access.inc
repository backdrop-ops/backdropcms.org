<?php
/**
 * @file
 * Plugin to provide access if a CiviCRM contact checksum is valid, or if a user is logged in.
 */

class CivicrmChecksumLayoutAccess extends LayoutAccessNegatable {

  /**
   * Constructor for a Layout access rule.
   */
  public function __construct($plugin_name, array $data = array()) {
    parent::__construct($plugin_name, $data);
    $this->settings += array(
      'validchecksum' => 1,
    );
  }

  /**
   * {@inheritdoc}
   */
  public function summary() {
    $summary = t('CiviCRM checksum rule is not configured');
    if ($this->settings['negate']) {
      switch ($this->settings['validchecksum']) {
        case 1:
          $summary = t('Invalid CiviCRM checksum');
          break;

        case 2:
          $summary = t('Invalid CiviCRM checksum and logged out user');
          break;

      }
    }
    else {
      switch ($this->settings['validchecksum']) {
        case 1:
          $summary = t('Valid CiviCRM checksum');
          break;

        case 2:
          $summary = t('Valid CiviCRM checksum or logged in user');
          break;

      }
    }
    return $summary;
  }

  /**
   * {@inheritdoc}
   */
  public function checkAccess() {
    if (!civicrm_initialize()) {
      return FALSE;
    }
    $valid_checksum = 0;
    // Determine if there is a valid checksum.
    $query_params = backdrop_get_query_parameters();
    $cid = check_plain($query_params['cid']);
    $cs = check_plain($query_params['cs']);
    if (!empty($cid) && !empty($cs)) {
      $valid_checksum = CRM_Contact_BAO_Contact_Utils::validChecksum($cid, $cs);
      if ($valid_checksum) {
        $status = 1;
      }
    }
    switch ($this->settings['validchecksum']) {
      case 1:
        if ($valid_checksum) {
          $status = 1;
        }
        break;

      case 2:
        if ($valid_checksum || user_is_logged_in()) {
          $status = 1;
        }
        break;

      default:
        $status = 0;
        break;

    }
    if ($this->settings['negate']) {
      return (!$status);
    }
    else {
      return ($status);
    }
  }

  /**
   * {@inheritdoc}
   */
  public function form(&$form, &$form_state) {
    parent::form($form, $form_state);
    $form['helptext'] = array(
      '#type' => 'markup',
      '#value' => '<div>' . t('Determine if a valid CiviCRM checksum link has been used.') . '</div>',
    );
    $form['validchecksum'] = array(
      '#type' => 'radios',
      '#title' => t('Show if'),
      '#options' => array(
        1 => t('A valid checksum link has been used to access the page'),
        2 => t('A valid checksum link has been used OR the user is logged in'),
      ),
      '#default_value' => $this->settings['validchecksum'],
    );
  }

  /**
   * {@inheritdoc}
   */
  public function formSubmit($form, &$form_state) {
    parent::formSubmit($form, $form_state);
    $this->settings['validchecksum'] = $form_state['values']['validchecksum'];
  }

}
