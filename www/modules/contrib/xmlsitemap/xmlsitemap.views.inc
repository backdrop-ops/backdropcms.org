<?php

/**
 * @file
 * Provides information about the data being made available for views.
 */

/**
 * Implements hook_views_data().
 *
 * Uses the xmlsitemap functions to load information about entity types that
 * are configured for xmlsitemap. Creates separate table aliases so that views
 * can display, filter and sort each entity type.
 */
function xmlsitemap_views_data() {
  $data = array();
  $link_info = xmlsitemap_get_link_info();
  foreach ($link_info as $info) {
    if (!empty($info['fieldable']) && $info['fieldable']) {
      $table_alias = 'xmlsitemap_' . $info['base table'];
      $data[$table_alias]['table'] = _xmlsitemap_table_data($info);
      $field_data = _xmlsitemap_field_data();
      foreach ($field_data as $field_name => $field) {
        $data[$table_alias][$field_name] = $field;
      }
    }
  }
  return $data;
}

/**
 * Table information.
 *
 * @param array $info
 *   Information about how the entity type is represented by xmlsitemap.
 *
 * @return array
 *   An array of table data based on the information about the entity type.
 *   Most importantly it returns join data for the table alias, which is
 *   different each time.
 */
function _xmlsitemap_table_data(array $info) {
  $table_data = array();
  $table_data['group'] = t('XML sitemap');
  // Gets the base table (node, user, etc.) from $info.
  $table_data['join'][$info['base table']] = array(
    'table' => 'xmlsitemap',
    'field' => 'id',
    // Gets the field name (nid, uid, etc.) from $info.
    'left_field' => $info['entity keys']['id'],
    'extra' => array(
      array(
        'field' => 'type',
        // Also filters on the entity type (node, user, etc.),
        // not to be confused with the base table name above.
        'value' => $info['type'],
        'operator' => '=',
      ),
    ),
  );
  return $table_data;
}

/**
 * Field information.
 *
 * @return array
 *   Returns the field information for four fields (status, status override,
 *   priority, priority override) that can be shown in views.  These are always
 *   the same no matter what entity type the view is showing.
 */
function _xmlsitemap_field_data() {
  $field_data = array();
  $field_data['status'] = array(
    'title' => t('Inclusion'),
    'help' => t('Is this included in the XML sitemap?'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
    ),
  );
  $field_data['status_override'] = array(
    'title' => t('Inclusion override'),
    'help' => t('Was the XML sitemap inclusion overridden, or left as the default?'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
    ),
  );
  $field_data['priority'] = array(
    'title' => t('Priority'),
    'help' => t('The XML sitemap priority.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $field_data['priority_override'] = array(
    'title' => t('Priority override'),
    'help' => t('Was the XML sitemap priority overridden, or left as the default?'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
    ),
  );
  return $field_data;
}
