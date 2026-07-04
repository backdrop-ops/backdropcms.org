<?php
/**
 * @file
 * Drush integration functions for the xmlsitemap_custom module.
 *
 * @ingroup xmlsitemap
 */

/**
 * Implements hook_drush_command().
 */
function xmlsitemap_custom_drush_command() {
  $items['xmlsitemap-custom-scan'] = array(
    'description' => 'Scan a directory for static files to include as custom XML sitemap links.',
    'arguments' => array(
      'directory' => 'The base directory to scan, without trailing slash.',
      'mask' => 'The preg_match() regular expression of the files to find.',
    ),
    'required-arguments' => TRUE,
    'callback' => 'drush_xmlsitemap_custom_scan',
    'backdrop dependencies' => array('xmlsitemap_custom'),
  );
  return $items;
}

/**
 * Scan a directory for static files to include via custom XML sitemap links.
 */
function drush_xmlsitemap_custom_scan($directory, $mask) {
  module_load_include('scan.inc', 'xmlsitemap_custom');
  drush_print(dt('About to scan @directory with @mask.', array(
    '@directory' => $directory,
    '@mask' => $mask,
  )));
  $results = xmlsitemap_custom_scan_directories(array($directory => $mask));
  drush_print(dt('Scanned @directory: added @add_count and removed @delete_count.', array(
    '@directory' => $directory,
    '@add_count' => count($results[$directory]['add']),
    '@delete_count' => count($results[$directory]['delete']),
  )));
}
