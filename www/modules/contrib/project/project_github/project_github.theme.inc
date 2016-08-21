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
  $clone_url = $url . '.git';
  $items = array(
    l(t('Project page'), $url),
    l(t('Issue Queue'), $url . '/issues'),
    l(t('Documentation'), $url . '/wiki'),
  );

  $list = theme('item_list', array('items' => $items, 'title' => t('GitHub')));

  $clone  = '<div class="github-clone">';
  $clone .= '<label class="github-clone-label">' . t('Clone URL') . '</label>';
  $clone .= '<input type="text" readonly="" aria-label="Clone this repository at ' . $clone_url . '" value="' . $clone_url . '">';
  $clone .= '</div>';

  return $list . $clone;
}
