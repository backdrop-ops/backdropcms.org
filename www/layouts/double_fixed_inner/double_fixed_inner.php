<?php
/**
 * @file
 * PHP functions for the drawer layout.
 */

/**
 * Prepare variables for the drawer layout template file.
 */
function template_preprocess_layout__double_fixed_inner(&$variables) {
  if ($variables['content']['sidebar'] && $variables['content']['drawer']) {
    $variables['classes'][] = 'layout-both-sidebars';
  }
  elseif ($variables['content']['sidebar'] || $variables['content']['drawer']) {
    $variables['classes'][] = 'layout-one-sidebar';

    if ($variables['content']['sidebar']) {
      $variables['classes'][] = 'layout-has-sidebar';
    }
    else {
      $variables['classes'][] = 'layout-has-drawer';
    }
  }
  else {
    $variables['classes'][] = 'layout-no-sidebars';
  }

  // Special handling for header image.
  $variables['main_attributes'] = array('class' => array('l-content'));
  if (arg(0) == 'user' && is_numeric(arg(1)) && !arg(2)) {
    // We are on the user profile page.
    $variables['main_attributes']['class'][] = 'account-page';
    // Check to see if there is a profile image.
    $account = user_load(arg(1)); // Entity cache should save us here?
    if (isset($account->field_header_photo[LANGUAGE_NONE][0]['uri'])) {
      // Generate an image at the correct size.
      $image = image_style_url('header', $account->field_header_photo[LANGUAGE_NONE][0]['uri']);
      $variables['main_attributes']['style'] = 'background-image: url(' . $image . ')';
      // Add an addidional class.
      $variables['main_attributes']['class'][] = 'has-background';
    }
  }
}
