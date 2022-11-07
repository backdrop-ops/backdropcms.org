<?php
/**
 * @file
 * Administrative page callbacks for the xmlsitemap_engines module.
 */

/**
 * Form builder; Administration settings form.
 */
function xmlsitemap_engines_settings_form() {
  $config = config('xmlsitemap.settings');
  // Build the list of support engines for the checkboxes options.
  $engines = xmlsitemap_engines_get_engine_info();
  $engine_options = array();
  foreach ($engines as $engine => $engine_info) {
    $engine_options[$engine] = $engine_info['name'];
  }
  asort($engine_options);

  $form['engines'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Submit the sitemap to the following engines'),
    '#default_value' => $config->get('engines'),
    '#options' => $engine_options,
  );
  $form['engines_minimum_lifetime'] = array(
    '#type' => 'select',
    '#title' => t('Do not submit more often than every'),
    '#options' => backdrop_map_assoc(array(
      3600,
      10800,
      21600,
      32400,
      43200,
      86400,
      172800,
      259200,
      604800,
      604800 * 2,
      604800 * 4,
    ), 'format_interval'),
    '#default_value' => $config->get('engines_minimum_lifetime', 86400),
  );
  $form['engines_submit_updated'] = array(
    '#type' => 'checkbox',
    '#title' => t('Only submit if the sitemap has been updated since the last submission.'),
    '#default_value' => $config->get('engines_submit_updated', TRUE),
  );
  $form['engines_custom_urls'] = array(
    '#type' => 'textarea',
    '#title' => t('Custom submission URLs'),
    '#description' => t('Enter one URL per line. The token [sitemap] will be replaced with the URL to your sitemap. For example: %example-before would become %example-after.', array(
      '%example-before' => 'https://example.com/ping?[sitemap]',
      '%example-after' => xmlsitemap_engines_prepare_url('https://example.com/ping?[sitemap]', url('sitemap.xml', array(
        'absolute' => TRUE,
      ))),
    )),
    '#default_value' => $config->get('custom_urls', ''),
    '#rows' => 2,
    '#wysiwyg' => FALSE,
    '#element_validate' => array('xmlsitemap_engines_validate_custom_urls'),
  );

  // Ensure the xmlsitemap_engines variable gets filterd to a simple array.
  $form['array_filter'] = array('#type' => 'value', '#value' => TRUE);

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submit handler for xmlsitemap_engines_settings_form().
 */
function xmlsitemap_engines_settings_form_submit($form, $form_state) {
  $config = config('xmlsitemap.settings');
  foreach ($form_state['values'] as $key => $value) {
    $config->set($key, $value);
  }
  $config->save();
  backdrop_set_message(t('The configuration options have been saved.'));
}

/**
 * Validate the custom submission URL element.
 */
function xmlsitemap_engines_validate_custom_urls($element, &$form_state) {
  $custom_urls = preg_split('/[\r\n]+/', $element['#value'], -1, PREG_SPLIT_NO_EMPTY);
  foreach ($custom_urls as $custom_url) {
    $url = xmlsitemap_engines_prepare_url($custom_url, '');
    if (!valid_url($url, TRUE)) {
      form_error($element, t('Invalid URL %url.', array('%url' => $custom_url)));
    }
  }
  $form_state['values']['custom_urls'] = implode("\n", $custom_urls);
}
