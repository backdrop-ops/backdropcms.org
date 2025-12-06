<?php
/**
 * @file
 * Plugin to provide access control based upon the current site language.
 */

class CivicrmLayoutAccess extends LayoutAccess {

  /**
   * Constructor for a Layout access rule.
   */
  public function __construct($plugin_name, array $data = array()) {
    parent::__construct($plugin_name, $data);
    $this->settings += array(
      'civicrm' => array(),
    );
  }

  /**
   * {@inheritdoc}
   */
  public function summary() {
    $civicrm = array(
      'admin' => t('Admin'),
      'public' => t('Public'),
    );
    $names = array();
    foreach ($this->settings['civicrm'] as $civi) {
      $names[] = $civicrm[$civi];
    }
    if (empty($names)) {
      return t('No CiviCRM pages');
    }
    return format_plural(count($names), 'CiviCRM pages are "@civicrm"', 'CiviCRM pages are either "@civicrm"', array('@civicrm' => implode(', ', $names)));
  }

  /**
   * {@inheritdoc}
   */
  public function checkAccess() {
    if (arg(0) != 'civicrm') {
      return FALSE;
    }
    if (arg(1) == 'upgrade' || arg(1) == 'ajax') {
      return FALSE;
    }
    if (!civicrm_initialize()) {
      return FALSE;
    }

    // Get the menu items.
    $args = explode('?', $_GET['q']);
    $path = $args[0];

    // Get the menu for above URL.
    $item = CRM_Core_Menu::get($path);

    // Check path for public or not and use layout if matches settings
    switch (CRM_Utils_Array::value('is_public', $item)) {
      case TRUE:
        if (in_array('public', $this->settings['civicrm'])) {
          return TRUE;
        }
        break;

      case FALSE:
        if (in_array('admin', $this->settings['civicrm'])) {
          return TRUE;
        }
        break;

      default:
        return FALSE;
    }
  }

  /**
   * {@inheritdoc}
   */
  public function form(&$form, &$form_state) {
    parent::form($form, $form_state);
    $options = array(
      'admin' => t('Admin'),
      'public' => t('Public'),
    );
    $form['civicrm'] = array(
      '#title' => t('CiviCRM pages'),
      '#type' => 'checkboxes',
      '#options' => $options,
      '#description' => t('Pass only if the current CiviCRM page is one of the following types.'),
      '#default_value' => $this->settings['civicrm'],
    );
  }

  /**
   * {@inheritdoc}
   */
  public function formSubmit($form, &$form_state) {
    parent::formSubmit($form, $form_state);
    $this->settings['civicrm'] = array_keys(array_filter($form_state['values']['civicrm']));
  }

}
