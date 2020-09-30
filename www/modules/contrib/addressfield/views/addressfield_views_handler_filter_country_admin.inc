<?php
  /**
   * Defines a filter handler that can display the administrative area name
   * instead of the code.
   */
class addressfield_views_handler_filter_country_admin extends views_handler_filter_in_operator {

  /**
   * init the handler with necessary data.
   * @param $view
   *   The $view object this handler is attached to.
   * @param $options
   *   The item from the database; the actual contents of this will vary
   *   based upon the type of handler.
   */
  function init(&$view, &$options) {
    parent::init($view, $options);
    $this->view = &$view;

    $this->country_field = $this->definition['additional fields'][0];
    $this->admin_field = $this->definition['additional fields'][1];
  }

  /**
   * Override value options.
   */
  function get_value_options() {
    $this->value_title = t('Country');
    $field = field_info_field($this->definition['field_name']);
    $this->value_options = _addressfield_country_options_list($field);
  }

  /**
   * Override value_form() to provide two selects: country & admin area.
   *
   * This should be overridden by all child classes and it must
   * define $form['value']
   *
   * @see options_form()
   */
  function value_form(&$form, &$form_state) {
    $path = backdrop_get_path('module', 'addressfield');
    $form['value'] = array(
      '#type' => 'container',
      '#attached' => array(
        'css' => array($path . '/css/addressfield-views.css'),
        'js' => array($path . '/js/addressfield-views.js'),
      ),
    );

    // Since multiple values is not allowed...
    $filters = explode('-', $this->value[0]);
    $default_country = (isset($filters[0])) ? $filters[0] : '';
    $default_admin = (isset($filters[1])) ? $filters[1] : '';

    $id = $this->options['expose']['identifier'];
    $field = field_info_field($this->definition['field_name']);
    $countries = _addressfield_country_options_list($field);
    $form['value'][$id . '-country'] = array(
      '#type' => 'select',
      '#title' => t('Country'),
      '#options' => $countries,
      '#default_value' => $default_country,
      '#attributes' => array('class' => array('addressfield-views-country')),
    );

    $admin_areas = array('' => t('Please select'));
    module_load_include('inc', 'addressfield', 'addressfield.administrative_areas');
    foreach ($countries as $code => $name) {
      if ($code != 'All') {
        $areas = array('' => t('- Any -'));
        $all_areas = addressfield_get_administrative_areas($code);
        // Limit options to values currently in use.
        $result = db_query("SELECT DISTINCT($this->admin_field) AS administrative_area FROM {$this->definition['table']} WHERE $this->country_field = :code ORDER BY $this->admin_field", array(':code' => $code))->fetchAllKeyed(0,0);
        if (!empty($result)) {
          foreach ($result as $record) {
            $areas[$record] = $all_areas[$record];
          }
        }
        if (is_array($areas)) {
          $admin_areas[$code] = $areas;
        }
      }
    }
    $form['value'][$id . '-administrative_area'] = array(
      '#type' => 'select',
      '#title' => t('State / Province'),
      '#options' => $admin_areas,
      '#default_value' => $default_admin,
      '#attributes' => array('class' => array('addressfield-views-admin-area')),
    );
  }

  /**
   * Available operators.
   */
  function operators() {
    return array(
      'in' => array(
        'title' => t('Is one of'),
        'short' => t('in'),
        'short_single' => t('='),
        'method' => 'op_simple',
        'values' => 1,
      ),
      'not in' => array(
        'title' => t('Is not one of'),
        'short' => t('not in'),
        'short_single' => t('<>'),
        'method' => 'op_simple',
        'values' => 1,
      ),
    );
  }

  /**
   * Modify the exposed form settings.
   */
  function expose_form(&$form, &$form_state) {
    parent::expose_form($form, $form_state);
    unset($form['expose']['reduce']);
    unset($form['expose']['multiple']);
  }

  /**
   * Sets value based on input.
   */
  function accept_exposed_input($input) {
    if (empty($this->options['exposed'])) {
      return TRUE;
    }


    if (!empty($this->options['expose']['use_operator']) && !empty($this->options['expose']['operator_id']) && isset($input[$this->options['expose']['operator_id']])) {
      $this->operator = $input[$this->options['expose']['operator_id']];
    }

    if (!empty($this->options['expose']['identifier'])) {
      $value = '';
      if (isset($input[$this->options['expose']['identifier'] . '-country'])) {
        $value .= $input[$this->options['expose']['identifier'] . '-country'];
      }
      if (isset($input[$this->options['expose']['identifier'] . '-administrative_area'])) {
        $value .= '-' . $input[$this->options['expose']['identifier'] . '-administrative_area'];
      }

      if (isset($value)) {
        $this->value = $value;
      }
      else {
        return FALSE;
      }
    }

    return TRUE;
  }

  /**
   * Perform any necessary changes to the form values prior to storage.
   * There is no need for this function to actually store the data.
   */
  function value_submit($form, &$form_state) {
    $value = $form['value']['location-country']['#value'];
    if (isset($form['value']['location-administrative_area']['#value']) && $form['value']['location-administrative_area']['#value'] != '') {
      $value .= '-' . $form['value']['location-administrative_area']['#value'];
    }
    $form_state['values']['options']['value'] = array($value);
  }

  /**
   * Query the DB based on value.
   */
  function query() {
    // Since multiple values is not allowed...
    $filters = explode('-', $this->value);

    $this->ensure_my_table();
    $this->query->add_where($this->options['group'], "$this->table_alias.$this->country_field", array($filters[0]), $this->operator);

    // Only add the where when a value is provided.
    if (isset($filters[1]) && !empty($filters[1])) {
      $this->query->add_where($this->options['group'], "$this->table_alias.$this->admin_field", array($filters[1]), $this->operator);
    }
  }
}
