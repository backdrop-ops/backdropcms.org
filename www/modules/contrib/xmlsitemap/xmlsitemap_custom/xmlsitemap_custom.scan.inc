<?php
/**
 * @file
 * Code for injecting custom XML sitemap entries for legacy stories.
 */

/**
 * Scan directories and keep matching subtrees of the custom XML sitemap.
 *
 * This will ensure that there are XML sitemap custom links for every file
 * that matches, and remove any custom links that point to files that don't
 * exist. Note that if you call this with a more restrictive mask for a given
 * directory, the scan might remove custom links that point to existing files
 * (they just don't match the given $mask).
 *
 * @param array $directories
 *   An associative array keyed by directory names where the values are
 *   preg_match() regular expressions of the files to find.
 *
 * @return array
 *   A nested array of results, keyed by directory name, where the values are
 *   associative arrays with the keys 'add' or 'delete' where the values are
 *   filenames of links that were added or deleted during the scan.
 *
 * @see file_scan_directory()
 */
function xmlsitemap_custom_scan_directories(array $directories) {
  $results = array();
  foreach ($directories as $directory => $mask) {
    $files = file_scan_directory($directory, $mask);
    $custom_links = db_select('xmlsitemap', 'x')
      ->fields('x', array('loc', 'id'))
      ->condition('x.type', 'custom')
      ->condition('x.subtype', $directory)
      ->execute()
      ->fetchAllAssoc('loc', PDO::FETCH_ASSOC);

    watchdog('xmlsitemap', 'Files: <pre>' . var_export($files, TRUE), WATCHDOG_NOTICE);
    watchdog('xmlsitemap', 'Custom: <pre>' . var_export($custom_links, TRUE), WATCHDOG_NOTICE);

    foreach (array_diff_key($files, $custom_links) as $missing => $info) {
      $link = array(
        'type' => 'custom',
        'subtype' => $directory,
        'loc' => $missing,
        'id' => db_query("SELECT MAX(id) FROM {xmlsitemap} WHERE type = 'custom'")->fetchField() + 1,
        'priority' => XMLSITEMAP_PRIORITY_DEFAULT,
        'lastmod' => 0,
        'changefreq' => 0,
        'changecount' => 0,
        'language' => LANGUAGE_NONE,
      );
      xmlsitemap_link_save($link);
      watchdog('xmlsitemap', 'Added a custom link for %loc.', array('%loc' => $link['loc']), WATCHDOG_NOTICE);
      $results[$directory]['add'][] = $link['loc'];
    }

    foreach (array_diff_key($custom_links, $files) as $extra => $info) {
      xmlsitemap_link_delete('custom', $custom_links[$extra]);
      $results[$directory]['delete'][] = $extra;
      watchdog('xmlsitemap', 'Deleted the custom link for %loc.', array('%loc' => $extra), WATCHDOG_NOTICE);
    }
  }
  return $results;
}
