<?php
/**
 * @file
 * Administrative interface for On The Web module.
 */

/**
 * Settings form.
 */
function on_the_web_settings_form($form, &$form_state) {
  $config = config('on_the_web.settings');
  $form = array();

  $form['settngs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Display Options'),
    '#collapsible' => TRUE,
  );

  $form['settngs']['type'] = array(
    '#type' => 'radios',
    '#title' => t('Icon type'),
    '#options' => array(
      'font' => t('Icon font tags'),
      'anchor' => t('SVGs as CSS background images'),
      'image' => t('Legacy image tags (may appear blurry)'),
    ),
    '#default_value' => $config->get('type'),
    'font' => array(
      '#description' => t('Options for icon font variations are provided below.'),
    ),
    'anchor' => array(
      '#description' => t('SVG icons will approximately match the icon fonts.'),
    ),
    'image' => array(
      '#description' => t('Note: Not all icons are available as legacy images. This option will be rmeoved in the future.'),
    ),
  );
  $form['settngs']['square'] = array(
    '#type' => 'checkbox',
    '#title' => t("Use 'square' icons when available."),
    '#default_value' => $config->get('square'),
    '#states' => array(
      'invisible' => array(
        ':input[name="type"]' => array('value' => 'image'),
      ),
    ),
  );

  $sources = array(
    'forkawesome' => array(
      'key' => '1.2.0',
      'label' => 'ForkAwesome version 1.2.0',
      'icons_url' => 'http://forkaweso.me/Fork-Awesome/icons',
      'description' => t('The FontAwesome Fork'),
    ),
    'fontawesome_5' => array(
      'key' => '5.x.x',
      'label' => 'FontAwesome version 5.x (requires registration)',
      'icons_url' => 'https://fontawesome.com/v5.15/icons',
      'description' => t('The latest version of FontAwesome. <a href=":faurl">FontAwesome account</a> Required.', array(':faurl' => 'https://fontawesome.com/start')),
    ),
    'fontawesome' => array(
      'key' => '4.7.0',
      'label' => 'FontAwesome version 4.7.0',
      'icons_url' => 'https://fontawesome.com/v4.7/icons',
      'description' => t('The last Open Source version of FontAwesome'),
    ),
  );
  
  $form['settngs']['version'] = array(
    '#type' => 'radios',
    '#title' => t('Icon Font and Version'),
    '#default_value' => $config->get('version'),
    '#states' => array(
      'visible' => array(
        ':input[name="type"]' => array('value' => 'font'),
      ),
    ),
  );

  $source_options = array();
  foreach ($sources as $source) {
    $icons_link = l('see all icons', $source['icons_url']);
    $source_options[$source['key']] = $source['label'] . ' &mdash; ' . $icons_link;
    $form['version']['key']['#description'] = $source['description'];
  }

  $form['settngs']['version']['#options'] = $source_options;

  $form['settngs']['kit_hash'] = array(
    '#type' => 'textfield',
    '#title' => t('FontAwesome 5.x Kit Hash'),
    '#field_prefix' => 'https://kit.fontawesome.com/',
    '#field_suffix' => '.js',
    '#size' => 20,
    '#default_value' => $config->get('kit_hash'),
    '#attributes' => array(
      'placeholder' => 'abcdefghij',
    ),
    '#description' => t('A 10-digit hash from the FontAwesome Kit code. <a href=":faurl">Register with FontAwesome</a> to generate a Kit.', array(':faurl' => '')),
    '#states' => array(
      'visible' => array(
        ':input[name="type"]' => array('value' => 'font'),
        ':input[name="version"]' => array('value' => '5.x.x'),
      ),
    ),
  );

  $form['settngs']['size'] = array(
    '#type' => 'radios',
    '#title' => t('Icon size'),
    '#options' => array(
      'sm' => t('Small: ~32px square'),
      'lg' => t('Large: ~64px square'),
    ),
    '#default_value' => $config->get('size'),
  );
  $form['settngs']['sitename'] = array(
    '#type' => 'radios',
    '#title' => 'Hover text selection',
    '#default_value' => (int)$config->get('sitename'),
    '#description' => t('This setting will update the title attribute for each social media link.'),
    '#options' => array(
      1 => t('Use the <strong><a href="!url">Site name</a></strong>, for example: <em>Find !us on Facebook</em>', array('!us' => config_get('system.core', 'site_name'), '!url' => url('admin/config/system/site-information'))),
      0 => t('Use <strong>us</strong>, for example: <em>Find us on Facebook</em>'),
      2 => t('Use <strong>me</strong>, for example: <em>Find me on Facebook</em>'),
    ),
  );
  $form['settngs']['target'] = array(
    '#type' => 'checkbox',
    '#title' => t('Open links in new windows'),
    '#default_value' => $config->get('target'),
    '#description' => t('Unchecking this box his will remove the target="_blank" attribute from each link.'),
  );

  $path = backdrop_get_path('module', 'on_the_web');

  // Get all the services.
  $services = on_the_web_get_services();
  foreach ($services as $service => $info) {
    $weight = $config->get($service . '_weight');

    $form['services'][$service] = array(
      '#tree' => TRUE,
      '#weight' => $weight,
    );

    $form['services'][$service]['name'] = array('#hidden' => $service);

    $form['services'][$service]['title'] = array('#markup' => $info['name']);

    $square = $config->get('square');
    $svg = array();
    if (isset($info['svg'])) {
      // Only use a square icon if it exists.
      if ($square == TRUE && isset($info['svg-square'])) {
        $src = base_path() . $path . '/images/svgs/' . $info['svg-square'];
      } 
      else {
        $src = base_path() . $path . '/images/svgs/' . $info['svg'];
      }
      $svg = array('#markup' => '<img src="' . $src . '" style="max-width: 2em; max-height: 2em;" />');
    }
    $form['services'][$service]['svg'] = $svg;

    
    $type = $config->get('type');
    if ($type == 'font') {
      $version = $config->get('version');
      $icon_classes = array();

      if ($version != '5.x.x') {
        $icon_classes = array('fa');
      }

      $icon_classes[] = 'fa-2x';
      // Only use a square icon if it exists.
      if ($square == TRUE && isset($info['fa-icon-square'])) {
        $icon_classes[] = $info['fa-icon-square'];
        if ($version == '5.x.x' && $service == 'linkedin') {
          // Exception for LinkedIn whicn uses a non-square icon in v5.
          $icon_classes[] = $info['fa-icon'];
        }
      }
      else {
        $icon_classes[] = $info['fa-icon'];
      }

      switch ($version) {
        case '1.2.0':
          $fork_icon = array();
          if (isset($info['fa-icon'])) {
            $itag = '<i aria-hidden="true" class="' . implode(' ', $icon_classes) . '"></i>';
            $fork_icon = array('#markup' => $itag);
          }
          $form['services'][$service]['fork'] = $fork_icon;

          $fork_url = 'https://cdn.jsdelivr.net/npm/fork-awesome@1.2.0/css/fork-awesome.min.css';
          $form['#attached']['css'][$fork_url] = array('type' => 'external');
          break;
        case '5.x.x':
          $hash = $config->get('kit_hash');
          if (!empty($hash)) {
            $font_js_url = 'https://kit.fontawesome.com/' . $hash . '.js';
            $form['#attached']['js'][$font_js_url] = array('type' => 'external');

            $fav5_icon = array();
            if (isset($info['fa-icon'])) {
              $icon_classes = array($info['v5style'], 'fa-2x', $info['fa-icon']);
              $itag = '<i aria-hidden="true" class="' . implode(' ', $icon_classes) . '"></i>';
              $fav5_icon = array('#markup' => $itag);
            }
            $form['services'][$service]['fav5'] = $fav5_icon;
          }
          break;
        case '4.7.0':
          $fav4_icon = array();
          if (isset($info['fa-icon'])) {
            $itag = '<i aria-hidden="true" class="' . implode(' ', $icon_classes) . '"></i>';
            $fav4_icon = array('#markup' => $itag);
          }
          $form['services'][$service]['fav4'] = $fav4_icon;

          $font_url = 'https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css';
          $form['#attached']['css'][$font_url] = array('type' => 'external');
          break;
      }
    }

    $legacy = array();
    if (isset($info['image'])) {
      $src = base_path() . $path . '/images/sm/' . $info['image'];
      $legacy = array('#markup' => '<img src="' . $src . '" />');
    }
    $form['services'][$service]['legacy'] = $legacy;

    $form['services'][$service]['weight'] = array(
      '#type' => 'weight',
      '#default_value' => $weight,
    );

    $form['services'][$service]['page'] = array(
      '#type' => 'textfield',
      '#default_value' => $config->get($service . '_page'),
    );
  }

  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Submit'));

  return $form;
}

/**
 * Submit handler for on_the_web_settings_form().
 */
function on_the_web_settings_form_submit($form, &$form_state) {
  $services = on_the_web_get_services();

  $config = config('on_the_web.settings');
  $config->set('type', $form_state['values']['type']);
  $config->set('square', $form_state['values']['square']);
  $config->set('version', $form_state['values']['version']);
  $config->set('kit_hash', $form_state['values']['kit_hash']);
  $config->set('size', $form_state['values']['size']);
  $config->set('sitename', $form_state['values']['sitename']);
  $config->set('target', $form_state['values']['target']);

  foreach ($services as $name => $info) {
    $config->set($name . '_page', $form_state['values'][$name]['page']);
    $config->set($name . '_weight', $form_state['values'][$name]['weight']);
  }

  $config->save();
}
