<?php
/**
 * @file
 * Plugin to provide access control based on CiviCRM path and query parameters.
 */

class CivicrmUrlLayoutAccess extends LayoutAccess {

  /**
   * Constructor for a Layout access rule.
   */
  public function __construct($plugin_name, array $data = array()) {
    parent::__construct($plugin_name, $data);
    $this->settings += array(
      'visibility_setting' => 1,
      'civicrm_urls' => '',
    );
  }

  /**
   * {@inheritdoc}
   */
  public function summary() {
    $summary = t('CiviCRM URLs:');
    $civicrm_urls = $this->settings['civicrm_urls'];
    $lines = array_map('trim', explode("\n", $civicrm_urls));
    $urls = implode(', ', $lines);
    if ($this->settings['visibility_setting']) {
      $summary .= ' ' . t('Allow access on @urls', array('@urls' => $urls));
    }
    else {
      $summary .= ' ' . t('Deny access on @urls', array('@urls' => $urls));
    }
    return $summary;
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

    $visibility_setting = $this->settings['visibility_setting'];
    $paths = array_map('trim', explode("\n", $this->settings['civicrm_urls']));
    foreach ($paths as $path) {
      $path_parts = explode('?', $path);
      if ($_GET['q'] != $path_parts[0]) {
        continue;
      }
      $query_parts = array();
      parse_str($path_parts[1], $query_parts);
      $match = TRUE;
      foreach ($query_parts as $key => $value) {
        $match &= (isset($_GET[$key]) && $_GET[$key] == $value);
      }
      if ($match) {
        return $visibility_setting;
      }
    }
    return !$visibility_setting;
  }

  /**
   * {@inheritdoc}
   */
  public function form(&$form, &$form_state) {
    parent::form($form, $form_state);

    $form['visibility_setting'] = array(
      '#type' => 'radios',
      '#title' => t('Visibility'),
      '#options' => array(
        1 => t('Allow access on the following pages'),
        0 => t('Allow access on all pages except the following pages'),
      ),
      '#default_value' => $this->settings['visibility_setting'],
      '#required' => TRUE,
    );
    $form['civicrm_urls'] = array(
      '#title' => t('CiviCRM URLs (path + query)'),
      '#type' => 'textarea',
      '#description' => t('Enter one path (and query parameters) per line. Example paths are "civicrm/event/info?id=1" for one event, or "civicrm/event/info" for all events. Each line starts with "civicrm", not including the domain. Query parameter order is irrelevant and extra query parameters in the page URL will be ignored. But all query parameters in each line must be present in the page URL in order to have a match.'),
      '#required' => TRUE,
      '#default_value' => $this->settings['civicrm_urls'],
    );
  }

  /**
   * {@inheritdoc}
   */
  public function formSubmit($form, &$form_state) {
    parent::formSubmit($form, $form_state);
    $this->settings['visibility_setting'] = $form_state['values']['visibility_setting'];
    $this->settings['civicrm_urls'] = $form_state['values']['civicrm_urls'];
  }

}
