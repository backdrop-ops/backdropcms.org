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

  $form['type'] = array(
    '#type' => 'radios',
    '#title' => t('Icon type'),
    '#options' => array(
      'anchor' => t('Background image on anchor'),
      'image' => t('Image tags in anchor'),
    ),
    '#default_value' => $config->get('type'),
  );
  $form['size'] = array(
    '#type' => 'radios',
    '#title' => t('Icon size'),
    '#options' => array(
      'sm' => t('Small: 32px square'),
      'lg' => t('Large: 64px square'),
    ),
    '#default_value' => $config->get('size'),
  );
  $form['target'] = array(
    '#type' => 'checkbox',
    '#title' => t('Open links in new windows'),
    '#default_value' => $config->get('target'),
    '#description' => t('Unchecking this box his will remove the target="_blank" attribute from each link.'),
  );
  $form['sitename'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use <a href="!url">Site name</a> in title attributes, for example: <em>Find !us on Facebook</em>', array('!us' => config_get('system.core', 'site_name'), '!url' => url('admin/config/system/site-information'))),
    '#default_value' => $config->get('sitename'),
    '#description' => t('Unchecking this box will update the title attributes for each link to use "us", for example: <em>Find us on Facebook</em>.'),
  );

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

    $form['services'][$service]['weight'] = array(
      '#type' => 'weight',
      '#default_value' => $weight,
    );

    $form['services'][$service]['page'] = array(
      '#type' => 'textfield',
      '#default_value' => $config->get($service . '_page'),
    );
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Submit'));

  return $form;
}

/**
 * Submit handler for on_the_web_settings_form().
 */
function on_the_web_settings_form_submit($form, &$form_state) {
  $services = on_the_web_get_services();

  $config = config('on_the_web.settings');
  $config->set('type', $form_state['values']['type']);
  $config->set('size', $form_state['values']['size']);
  $config->set('sitename', $form_state['values']['sitename']);
  $config->set('target', $form_state['values']['target']);

  foreach ($services as $name => $info) {
    $config->set($name . '_page', $form_state['values'][$name]['page']);
    $config->set($name . '_weight', $form_state['values'][$name]['weight']);
  }

  $config->save();
}
