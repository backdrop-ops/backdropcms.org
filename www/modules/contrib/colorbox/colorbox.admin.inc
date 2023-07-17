<?php
/**
 * @file
 * Administrative page callbacks for the colorbox module.
 */

/**
 * General configuration form for controlling the colorbox behaviour.
 */
function colorbox_admin_settings() {
  $module_path = backdrop_get_path('module', 'colorbox');
  $colorbox_path = $module_path . '/libraries/colorbox';
  backdrop_add_js($module_path . '/js/colorbox_admin_settings.js', array('preprocess' => FALSE));

  $form['colorbox_extra_features'] = array(
    '#type' => 'fieldset',
    '#title' => t('Extra features'),
  );
  $form['colorbox_extra_features']['colorbox_load'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Colorbox load'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_load'),
    '#description' => t('This enables custom links that can open forms and paths in a Colorbox. Add the class "colorbox-load" to the link and build the url like this for paths "[path]?width=500&height=500&iframe=true" or "[path]?width=500&height=500" if you don\'t want an iframe. Other modules may activate this for easy Colorbox integration.'),
  );
  $form['colorbox_extra_features']['colorbox_inline'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Colorbox inline'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_inline'),
    '#description' => t('This enables custom links that can open inline content in a Colorbox. Add the class "colorbox-inline" to the link and build the url like this "?width=500&height=500&inline=true#id-of-content". Other modules may activate this for easy Colorbox integration.'),
  );

  $form['colorbox_dompurify'] = array(
    '#type' => 'fieldset',
    '#title' => t('HTML Captions'),
    '#description' => t('The <a href="!dompurify_link">DOMPurify</a> ' .
      'library is necessary if you want to use HTML in Colorbox captions. ' .
      'Without it, all captions will be treated as plain text.',
       array(
         '!dompurify_link' => 'https://github.com/cure53/DOMPurify',
       )
    ),
  );
  $form['colorbox_dompurify']['colorbox_dompurify_include'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow HTML in Colorbox captions'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_dompurify_include'),
    '#description' => t('This setting will add the DOMPurity library.'),
  );

  if (module_exists('insert')) {
    $form['colorbox_insert_module'] = array(
      '#type' => 'fieldset',
      '#title' => t('Insert module settings'),
    );
    $image_styles = image_style_options(FALSE);
    $form['colorbox_insert_module']['colorbox_image_style'] = array(
      '#type' => 'select',
      '#title' => t('Image style'),
      '#empty_option' => t('None (original image)'),
      '#options' => $image_styles,
      '#default_value' => config_get('colorbox.settings', 'colorbox_image_style'),
      '#description' => t('Select which image style to use for viewing images in the colorbox.'),
    );
    $form['colorbox_insert_module']['colorbox_insert_gallery'] = array(
      '#type' => 'radios',
      '#title' => t('Insert image gallery'),
      '#default_value' => config_get('colorbox.settings', 'colorbox_insert_gallery'),
      '#options' => array(0 => t('Per page gallery'), 3 => t('No gallery')),
      '#description' => t('Should the gallery be all images on the page (default) or disabled.'),
    );
  }

  $form['colorbox_custom_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Styles and options'),
  );
  $colorbox_styles = array(
    'default' => t('Default'),
    'plain' => t('Plain (mainly for images)'),
    'stockholmsyndrome' => t('Stockholm Syndrome'),
    $colorbox_path . '/example1' => t('Example 1'),
    $colorbox_path . '/example2' => t('Example 2'),
    $colorbox_path . '/example3' => t('Example 3'),
    $colorbox_path . '/example4' => t('Example 4'),
    $colorbox_path . '/example5' => t('Example 5'),
    'none' => t('None'),
  );
  $form['colorbox_custom_settings']['colorbox_style'] = array(
    '#type' => 'select',
    '#title' => t('Style'),
    '#options' => $colorbox_styles,
    '#default_value' => config_get('colorbox.settings', 'colorbox_style'),
    '#description' => t('Select "None" if Colorbox styles are provided by your theme.'),
  );

  $colorbox_custom_settings_activate = config_get('colorbox.settings', 'colorbox_custom_settings_activate');
  $form['colorbox_custom_settings']['colorbox_custom_settings_activate'] = array(
    '#type' => 'radios',
    '#title' => t('Colorbox options'),
    '#options' => array(0 => t('Use default options'), 1 => t('Use custom options')),
    '#default_value' => $colorbox_custom_settings_activate,
    '#prefix' => '<div class="colorbox-custom-settings-activate">',
    '#suffix' => '</div>',
  );

  $js_hide_activate = $colorbox_custom_settings_activate ? '' : ' js-hide';
  $form['colorbox_custom_settings']['wrapper_start'] = array(
    '#markup' => '<div class="colorbox-custom-settings' . $js_hide_activate . '">',
  );

  $form['colorbox_custom_settings']['colorbox_transition_type'] = array(
    '#type' => 'radios',
    '#title' => t('Transition type'),
    '#options' => array(
      'elastic' => t('Elastic'),
      'fade' => t('Fade'),
      'none' => t('None'),
    ),
    '#default_value' => config_get('colorbox.settings', 'colorbox_transition_type'),
  );
  $form['colorbox_custom_settings']['colorbox_transition_speed'] = array(
    '#type' => 'select',
    '#title' => t('Transition speed'),
    '#field_suffix' => t('milliseconds'),
    '#options' => backdrop_map_assoc(array(
      100,
      150,
      200,
      250,
      300,
      350,
      400,
      450,
      500,
      550,
      600,
    )),
    '#default_value' => config_get('colorbox.settings', 'colorbox_transition_speed'),
    '#description' => t('Sets the speed of the fade and elastic transitions.'),
  );
  $form['colorbox_custom_settings']['colorbox_opacity'] = array(
    '#type' => 'select',
    '#title' => t('Opacity'),
    '#options' => backdrop_map_assoc(array(
      '0',
      '0.10',
      '0.15',
      '0.20',
      '0.25',
      '0.30',
      '0.35',
      '0.40',
      '0.45',
      '0.50',
      '0.55',
      '0.60',
      '0.65',
      '0.70',
      '0.75',
      '0.80',
      '0.85',
      '0.90',
      '0.95',
      '1',
    )),
    '#default_value' => config_get('colorbox.settings', 'colorbox_opacity'),
    '#description' => t('The overlay opacity level. Range: 0 to 1.'),
  );
  $form['colorbox_custom_settings']['colorbox_text_current'] = array(
    '#type' => 'textfield',
    '#title' => t('Text for the content group or gallery count'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_text_current'),
    '#size' => 30,
    '#description' => t('The placeholders {current} and {total} will be replaced with actual numbers at runtime.'),
  );
  $form['colorbox_custom_settings']['colorbox_text_previous'] = array(
    '#type' => 'textfield',
    '#title' => t('Text for the previous button'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_text_previous'),
    '#size' => 30,

  );
  $form['colorbox_custom_settings']['colorbox_text_next'] = array(
    '#type' => 'textfield',
    '#title' => t('Text for the next button'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_text_next'),
    '#size' => 30,
  );
  $form['colorbox_custom_settings']['colorbox_text_close'] = array(
    '#type' => 'textfield',
    '#title' => t('Text for the close button'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_text_close'),
    '#size' => 30,
    '#description' => t('The "Esc" key will also close Colorbox.'),
  );
  $form['colorbox_custom_settings']['colorbox_overlayclose'] = array(
    '#type' => 'checkbox',
    '#title' => t('Overlay close'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_overlayclose'),
    '#description' => t('Enable closing Colorbox by clicking on the background overlay.'),
  );
  $form['colorbox_custom_settings']['colorbox_returnfocus'] = array(
    '#type' => 'checkbox',
    '#title' => t('Return focus'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_returnfocus'),
    '#description' => t('Return focus when Colorbox exits to the element it was launched from.'),
  );
  $form['colorbox_custom_settings']['colorbox_maxwidth'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum width for loaded content'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_maxwidth'),
    '#size' => 30,
    '#description' => t('Example: "100%", 500, "500px".'),
  );
  $form['colorbox_custom_settings']['colorbox_maxheight'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum height for loaded content'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_maxheight'),
    '#size' => 30,
    '#description' => t('Example: "100%", 500, "500px".'),
  );
  $form['colorbox_custom_settings']['colorbox_initialwidth'] = array(
    '#type' => 'textfield',
    '#title' => t('Initial width, prior to any content being loaded'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_initialwidth'),
    '#size' => 30,
    '#description' => t('Example: "100%", 500, "500px".'),
  );
  $form['colorbox_custom_settings']['colorbox_initialheight'] = array(
    '#type' => 'textfield',
    '#title' => t('Initial height, prior to any content being loaded'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_initialheight'),
    '#size' => 30,
    '#description' => t('Example: "100%", 500, "500px".'),
  );
  $form['colorbox_custom_settings']['colorbox_fixed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Fixed position'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_fixed'),
    '#description' => t('If the Colorbox should be displayed in a fixed position within the visitor\'s viewport or relative to the document.'),
  );

  $form['colorbox_custom_settings']['colorbox_scrolling'] = array(
    '#type' => 'radios',
    '#title' => t('Scrollbars'),
    '#options' => array(1 => t('On'), 0 => t('Off')),
    '#default_value' => config_get('colorbox.settings', 'colorbox_scrolling'),
    '#description' => t('If set to Off, Colorbox will hide scrollbars for overflowing content. This could be used in conjunction with the resize method for a smoother transition if you are appending content to an already open instance of Colorbox.'),
  );

  $form['colorbox_custom_settings']['colorbox_slideshow_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Slideshow settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['colorbox_custom_settings']['colorbox_slideshow_settings']['colorbox_slideshow'] = array(
    '#type' => 'radios',
    '#title' => t('Slideshow'),
    '#options' => array(0 => t('Off'), 1 => t('On')),
    '#default_value' => config_get('colorbox.settings', 'colorbox_slideshow'),
    '#description' => t('An automatic slideshow to a content group / gallery.'),
    '#prefix' => '<div class="colorbox-slideshow-settings-activate">',
    '#suffix' => '</div>',
  );

  $js_hide_slideshow = config_get('colorbox.settings', 'colorbox_slideshow') ? '' : ' js-hide';
  $form['colorbox_custom_settings']['colorbox_slideshow_settings']['wrapper_start'] = array(
    '#markup' => '<div class="colorbox-slideshow-settings' . $js_hide_slideshow . '">',
  );

  $form['colorbox_custom_settings']['colorbox_slideshow_settings']['colorbox_slideshowauto'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically start to play the slideshow'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_slideshowauto'),
  );
  $form['colorbox_custom_settings']['colorbox_slideshow_settings']['colorbox_slideshowspeed'] = array(
    '#type' => 'select',
    '#title' => t('Slideshow speed'),
    '#field_suffix' => t('milliseconds'),
    '#options' => backdrop_map_assoc(array(
      1000,
      1500,
      2000,
      2500,
      3000,
      3500,
      4000,
      4500,
      5000,
      5500,
      6000,
    )),
    '#default_value' => config_get('colorbox.settings', 'colorbox_slideshowspeed'),
  );
  $form['colorbox_custom_settings']['colorbox_slideshow_settings']['colorbox_text_start'] = array(
    '#type' => 'textfield',
    '#title' => t('Start button text'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_text_start'),
    '#size' => 30,
  );
  $form['colorbox_custom_settings']['colorbox_slideshow_settings']['colorbox_text_stop'] = array(
    '#type' => 'textfield',
    '#title' => t('Stop button text'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_text_stop'),
    '#size' => 30,
  );

  $form['colorbox_custom_settings']['colorbox_slideshow_settings']['wrapper_stop'] = array(
    '#markup' => '</div>',
  );

  $form['colorbox_custom_settings']['wrapper_stop'] = array(
    '#markup' => '</div>',
  );

  $form['colorbox_advanced_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['colorbox_advanced_settings']['colorbox_unique_token'] = array(
    '#type' => 'radios',
    '#title' => t('Unique per-request gallery token'),
    '#options' => array(1 => t('On'), 0 => t('Off')),
    '#default_value' => config_get('colorbox.settings', 'colorbox_unique_token'),
    '#description' => t('If On (default), Colorbox will add a unique per-request token to the gallery id to avoid images being added manually to galleries. The token was added as a security fix but some see the old behavoiur as an feature and this settings makes it possible to remove the token.'),
  );
  $form['colorbox_advanced_settings']['colorbox_mobile_detect'] = array(
    '#type' => 'radios',
    '#title' => t('Mobile detection'),
    '#options' => array(1 => t('On'), 0 => t('Off')),
    '#default_value' => config_get('colorbox.settings', 'colorbox_mobile_detect'),
    '#description' => t('If On (default), Colorbox will not be active for devices with the max width set below.'),
  );
  $form['colorbox_advanced_settings']['colorbox_mobile_device_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Device width'),
    '#default_value' => config_get('colorbox.settings', 'colorbox_mobile_device_width'),
    '#size' => 30,
    '#description' => t('Set the mobile device max width. Default: 480px.'),
    '#states' => array(
      'visible' => array(
        ':input[name="colorbox_mobile_detect"]' => array('value' => '1'),
      ),
    ),
  );
  $form['colorbox_advanced_settings']['colorbox_caption_trim'] = array(
    '#type' => 'radios',
    '#title' => t('Caption shortening'),
    '#options' => array(0 => t('Default'), 1 => t('Yes')),
    '#default_value' => config_get('colorbox.settings', 'colorbox_caption_trim'),
    '#description' => t('If the caption should be made shorter in the Colorbox to avoid layout problems. The default is to shorten for the example styles, they need it, but not for other styles.'),
  );
  $form['colorbox_advanced_settings']['colorbox_caption_trim_length'] = array(
    '#type' => 'select',
    '#title' => t('Caption max length'),
    '#options' => backdrop_map_assoc(array(
      40,
      45,
      50,
      55,
      60,
      70,
      75,
      80,
      85,
      90,
      95,
      100,
      105,
      110,
      115,
      120,
    )),
    '#default_value' => config_get('colorbox.settings', 'colorbox_caption_trim_length'),
    '#states' => array(
      'visible' => array(
        ':input[name="colorbox_caption_trim"]' => array('value' => '1'),
      ),
    ),
  );
  $form['colorbox_advanced_settings']['colorbox_visibility'] = array(
    '#type' => 'radios',
    '#title' => t('Show Colorbox on specific pages'),
    '#options' => array(
      0 => t('All pages except those listed'),
      1 => t('Only the listed pages'),
    ),
    '#default_value' => config_get('colorbox.settings', 'colorbox_visibility'),
  );
  $form['colorbox_advanced_settings']['colorbox_pages'] = array(
    '#type' => 'textarea',
    '#title' => '<span class="element-invisible">' . t('Pages') . '</span>',
    '#default_value' => config_get('colorbox.settings', 'colorbox_pages'),
    '#description' => t("Specify pages by using their paths. Enter one path per line. The '*' character is a wildcard, and use <code>%front</code> for the front page. <p><a href=\"#\" class=\"colorbox-reset-specific-pages-default\">Reset to default</a></p>", array(
      '%front' => '<front>',
    )),
  );
  $form['colorbox_advanced_settings']['colorbox_compression_type'] = array(
    '#type' => 'radios',
    '#title' => t('Choose Colorbox compression level'),
    '#options' => array(
      'minified' => t('Production (Minified)'),
      'source' => t('Development (Uncompressed Code)'),
    ),
    '#default_value' => config_get('colorbox.settings', 'colorbox_compression_type'),
  );

  // Add a submit handler.
  $form['#submit'][] = 'colorbox_admin_settings_submit';
  // Add a submit button.
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Form submission handler for colorbox_admin_settings().
 */
function colorbox_admin_settings_submit($form, &$form_state) {
  $config = config('colorbox.settings');
  foreach ($form_state['values'] as $key => $value) {
    $config->set($key, $value);
  }

  $config->save();
  backdrop_set_message(t('The configuration options have been saved.'));
}
