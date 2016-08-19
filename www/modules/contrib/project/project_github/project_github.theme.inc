<?php
/**
 * @file
 * Theme functions for Project GitHub module.
 */

/**
 * GitHub project information for node page.
 */
function theme_github_info($variables) {
  $url = 'https://github.com/' . $variables['github_path'];
  $items = array(
    l(t('Project page'), $url),
    l(t('Issue Queue'), $url . '/issues'),
    l(t('Documentation'), $url . '/wiki'),
  );

  return theme('item_list', array('items' => $items, 'title' => t('GitHub')));
}
