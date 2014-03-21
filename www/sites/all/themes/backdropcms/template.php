<?php

/**
 * @file
 */

/**
 * Preprocess functions.
 */

/**
 * Prepares variables for page.tpl.php
 */
function backdropcms_preprocess_page(&$variables) {
  foreach($variables['main_menu'] as $key => $item) {
    $variables['main_menu'][$key]['title'] = t($variables['main_menu'][$key]['title']) . ' <i class="fa fa-chevron-right fa-fw"></i>';
    $variables['main_menu'][$key]['html'] = TRUE;
  }
}

/**
 * Prepares variables for block.tpl.php
 */
function backdropcms_preprocess_block(&$variables) {

}

/**
 * Theme overrides.
 */
