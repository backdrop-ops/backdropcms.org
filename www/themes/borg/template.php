<?php
/**
 * @file
 * Theme function overrides.
 */

/*******************************************************************************
 * Preprocess functions: prepare variables for templates.
 ******************************************************************************/

/**
 * Prepares variables for page.tpl.php
 */
function borg_preprocess_page() {
  // Add the Source Sans Pro font.
  drupal_add_css('https://fonts.googleapis.com/css?family=Source+Sans+Pro:300,400,700', array('type' => 'external'));
  // Add FontAwesome.
  drupal_add_css('https://maxcdn.bootstrapcdn.com/font-awesome/4.3.0/css/font-awesome.min.css', array('type' => 'external'));
}

/*******************************************************************************
 * Theme function overrides.
 ******************************************************************************/
