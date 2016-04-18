<?php
/**
 * @file
 * Contains \addressfield_views_handler_argument_countryname.
 */

/**
 * A views argument (contextual filter) handler which filters views results by
 * the name of a country instead of it's abbreviation.
 *
 * This contextual filter handler assumes that the value of the argument passed
 * into it (typically via the URL) is the same language that the request is
 * being executed with. This is due to the way that core maps country names and
 * abbreviations (see _country_get_predefined_list()).
 *
 * In other words, if the language used during a request is not determined by a
 * portion of the URL, this filter will probably not work for all countries.
 *
 * Here is an example...
 *
 * In French, the United States is known as "États-Unis".
 *
 * If a site is available in both English and French, AND the site is set up in
 * such a way that the language used during the request is determined by a
 * part of the page path (e.g.: English is used if the URL looks like
 * "/en/{path}" and French is used if the URL looks like "/fr/{path}"), then
 * this filter should work with no trouble (e.g.: both
 * "/en/locations/United States" and "/fr/locations/États-Unis" will show the
 * same list of results).
 *
 * However, if a site is available in both English and French, and the site is
 * set up to use cookies to determine the language of the request, then users
 * will experience problems: a request to * "/locations/United States" will show
 * no results if the cookies tell Drupal to perform the request in French.
 */
class addressfield_views_handler_argument_countryname extends views_handler_argument_string {
  /**
   * {@inheritdoc}
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // Notify the user that the language that the text in the URL must be in the
    // language used while parsing the request.
    $form['addressfield_countryname_warning'] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('messages', 'warning')),
      '#weight' => -50,
    );
    $form['addressfield_countryname_warning']['intro'] = array(
      '#type' => 'item',
      '#theme_wrappers' => 'p',
      '#markup' => t('If your site is available in more than one language, you could run into problems with this contextual filter if your site uses cookies to determine the language of the request. It will only work properly if a portion of the URL determines the language of the request.'),
    );
    $form['addressfield_countryname_warning']['more_info'] = array(
      '#type' => 'item',
      '#prefix' => '<p>',
      '#suffix' => '</p>',
      '#markup' => t('Due to the way country abbreviations are mapped to country names, this filter assumes that the value passed in via the URL is written in the same language as the request that is being executed.'),
    );
  }

  /**
   * Add pre-render function so views doesn't explode.
   *  @todo find out why this is needed and what it should be doing.
   */
  function pre_render() {
    // Do nothing?
  }

  /**
   * {@inheritdoc}
   */
  function query($group_by = FALSE) {
    // Transform dashes in arguments.
    $argument = $this->argument;
    if (!empty($this->options['transform_dash'])) {
      $argument = strtr($argument, '-', ' ');
    }

    // Look up countries by name.
    include_once BACKDROP_ROOT . '/core/includes/locale.inc';
    $countries = country_get_list();
    $country_by_name = array_flip($countries);
    if (isset($country_by_name[$this->argument])) {
      $this->argument = $country_by_name[$this->argument];
    }

    return parent::query();
  }
}
