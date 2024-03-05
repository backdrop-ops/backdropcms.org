<?php

/**
 * @file
 * Webform Link component
 */

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_link() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'required' => 0,
    'extra' => array(
      'disabled' => 0,
      'width' => '',
      'title_display' => 0,
      'description' => '',
      'description_above' => FALSE,
      'placeholder' => '',
      'attributes' => array(),
      'private' => FALSE,
      'analysis' => FALSE,
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_link($component) {
  $form = array();
  $form['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Default URL'),
    '#default_value' => $component['value'],
    '#description' => t('The default URL of the field.') . theme('webform_token_help'),
    '#size' => 60,
    '#maxlength' => 1024,
    '#weight' => 0,
  );
  $form['display']['width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#default_value' => $component['extra']['width'],
    '#description' => t('Width of the textfield.') . ' ' . t('Leaving blank will use the default size.'),
    '#size' => 5,
    '#maxlength' => 10,
    '#parents' => array('extra', 'width'),
  );
  $form['display']['placeholder'] = array(
    '#type' => 'textfield',
    '#title' => t('Placeholder'),
    '#default_value' => $component['extra']['placeholder'],
    '#description' => t('The placeholder will be shown in the field until the user starts entering a value.') . ' ' . t('Often used for example values, such as "https://example.com".'),
    '#parents' => array('extra', 'placeholder'),
  );
  $form['display']['disabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disabled'),
    '#return_value' => 1,
    '#description' => t('Make this field non-editable. Useful for displaying default value. Changeable via JavaScript or developer tools.'),
    '#weight' => 11,
    '#default_value' => $component['extra']['disabled'],
    '#parents' => array('extra', 'disabled'),
  );
  return $form;
}
/**
 * Implements _webform_theme_component().
 */
function _webform_theme_link() {
  return array(
    'webform_link' => array(
      'render element' => 'element',
      'file' => 'components/link.inc',
    ),
    'webform_display_link' => array(
      'render element' => 'element',
      'file' => 'components/link.inc',
    ),
  );
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_link($component, $value = NULL, $filter = TRUE) {
  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;

  $element = array(
    '#type' => 'webform_link',
    '#title' => $filter ? webform_filter_xss($component['name']) : $component['name'],
    '#title_display' => $component['extra']['title_display'] ? $component['extra']['title_display'] : 'before',
    '#default_value' => $filter ? webform_replace_tokens($component['value'], $node) : $component['value'],
    '#required' => $component['required'],
    '#weight' => $component['weight'],
    '#description' => $filter ? webform_filter_descriptions($component['extra']['description'], $node) : $component['extra']['description'],
    '#translatable' => array('title', 'description', 'placeholder'),
    '#theme_wrappers' => array('webform_element'),
    '#element_validate' => array('webform_link_validate'),
  );

  if ($component['required']) {
    $element['#attributes']['required'] = 'required';
  }

  // Add a link class for identifying the difference from normal textfields.
  $element['#attributes']['class'][] = 'link';

  if (isset($value[0])) {
    $element['#default_value'] = $value[0];
  }

  if ($component['extra']['placeholder']) {
    $element['#attributes']['placeholder'] = $component['extra']['placeholder'];
  }

  if ($component['extra']['disabled']) {
    if ($filter) {
      $element['#attributes']['readonly'] = 'readonly';
    }
    else {
      $element['#disabled'] = TRUE;
    }
  }

  // Change the 'width' option to the correct 'size' option.
  if ($component['extra']['width'] > 0) {
    $element['#size'] = $component['extra']['width'];
  }

  return $element;
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_link($component, $value, $format = 'html') {
  $element = array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#theme' => 'webform_display_link',
    '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
    '#format' => $format,
    '#value' => l($value[0], $value[0]),
    '#translatable' => array('title', 'placeholder'),
  );
  return $element;
}

/**
 * Theme function to render a link component.
 */
function theme_webform_link($variables) {
  $element = $variables['element'];

  // This IF statement is mostly in place to allow our tests to set type="text"
  // because SimpleTest does not support type="link".
  if (!isset($element['#attributes']['type'])) {
    $element['#attributes']['type'] = 'url';
  }

  // Convert properties to attributes on the element if set.
  foreach (array('id', 'name', 'value', 'size') as $property) {
    if (isset($element['#' . $property]) && $element['#' . $property] !== '') {
      $element['#attributes'][$property] = $element['#' . $property];
    }
  }
  _form_set_class($element, array('form-text', 'form-url'));

  return '<input' . backdrop_attributes($element['#attributes']) . ' />';
}

/**
 * Custom theme function for collected link.
 */
function theme_webform_display_link($variables) {
  $element = $variables['element'];
  return empty($element['#value']) ? ' ' : $element['#value'];
}

/**
 * Implements _webform_table_component().
 */
function _webform_table_link($component, $value) {
  return l($value[0], $value[0]);
}

/**
 * Implements _webform_analysis_component().
 */
function _webform_analysis_link($component, $sids = array(), $single = FALSE, $join = NULL) {
  $query = db_select('webform_submitted_data', 'wsd', array('fetch' => PDO::FETCH_ASSOC))
    ->fields('wsd', array('no', 'data'))
    ->condition('wsd.nid', $component['nid'])
    ->condition('wsd.cid', $component['cid']);

  if (count($sids)) {
    $query->condition('wsd.sid', $sids, 'IN');
  }

  if ($join) {
    $query->innerJoin($join, 'ws2_', 'wsd.sid = ws2_.sid');
  }

  $nonblanks = 0;
  $submissions = 0;
  $wordcount = 0;

  $result = $query->execute();
  foreach ($result as $data) {
    if (backdrop_strlen(trim($data['data'])) > 0) {
      $nonblanks++;
      $wordcount += str_word_count(trim($data['data']));
    }
    $submissions++;
  }

  $rows[0] = array(t('Left Blank'), ($submissions - $nonblanks));
  $rows[1] = array(t('User entered value'), $nonblanks);

  $other[0] = array(
    t('Average submission length in words (ex blanks)'),
    ($nonblanks != 0 ? number_format($wordcount / $nonblanks, 2) : '0')
  );

  return array(
    'table_rows' => $rows,
    'other_data' => $other,
  );
}

/**
 * Implements _webform_csv_headers_component().
 */
function _webform_csv_headers_link($component, $export_options) {
  $header = array();
  $header[0] = '';
  $header[1] = '';
  $header[2] = $export_options['header_keys'] ? $component['form_key'] : $component['name'];
  return $header;
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_csv_data_link($component, $export_options, $value) {
  return empty($value[0]) ? '' : $value[0];
}

/**
 * Form validation handler.
 *
 * If a link was submitted, check that the URL is valid.
 */
function webform_link_validate($element, &$form_state) {
  if ($element['#value'] && !valid_url($element['#value'], TRUE)) {
    $message = t('%url is not a valid URL.', array('%url' => $element['#value']));
    form_error($element, $message);
  }
}
