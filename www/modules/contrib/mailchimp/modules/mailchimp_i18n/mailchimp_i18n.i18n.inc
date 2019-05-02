<?php

/**
 * @file
 * i18n_string integration based upon the Entity API i18n.
 */

/**
 * Mailchimp i18n integration controller.
 */
class MailchimpI18nStringController extends EntityDefaultI18nStringController {

  /**
   * Overridden to customize i18n object info.
   *
   * @see EntityDefaultI18nStringController::hook_object_info()
   */
  public function hook_object_info() {
    $info = parent::hook_object_info();
    $info['mailchimp_signup']['class'] = 'MailchimpI18nStringObjectWrapper';
    return $info;
  }

}

/**
 * Custom I18n String object wrapper.
 *
 * Register custom properties per Mailchimp signup entity.
 */
class MailchimpI18nStringObjectWrapper extends i18n_string_object_wrapper {

  /**
   * Get translatable properties.
   */
  protected function build_properties() {
    $strings = parent::build_properties();
    $properties = array();

    $title_mapping = array(
      'description' => t('Description'),
      'submit_button' => t('Submit Button Label'),
      'confirmation_message' => t('Confirmation Message'),
    );

    // Add in translations for all settings and mergefields.
    foreach ($this->object->settings as $settings_key => $setting) {
      if (is_array($setting)) {
        $mergefields = array_filter($setting);
        foreach ($mergefields as $tag => $mergefield_settings) {
          $properties['mergefield:' . $tag] = array(
            'title' => $mergefield_settings->name,
            'string' => $mergefield_settings->name,
          );
        }
      }
      if (isset($title_mapping[$settings_key])) {
        $properties[$settings_key] = array(
          'title' => $title_mapping[$settings_key],
          'string' => $setting,
        );
      }
    }

    $strings[$this->get_textgroup()]['mailchimp_signup'][$this->object->name] += $properties;
    return $strings;
  }

}
