<?php
/**
 * @file
 * Contains the SearchApiMultiRowEntityView class.
 */

/**
 * Plugin class for displaying Views results with entity_view.
 */
class SearchApiMultiRowEntityView extends views_plugin_row {

  /**
   * The results of the query, wrapped in metadata wrappers.
   *
   * @var EntityMetadataWrapper[]
   */
  protected $wrappers;

  /**
   * The indexes used in the query.
   *
   * The indexes in this array are grouped by those that contain entities (in
   * the nested "entity_types" array and those that don't ("incompatibles" key).
   *
   * @var array
   */
  protected $indexes = NULL;

  /**
   * {@inheritdoc}
   */
  public function init(&$view, &$display, $options = NULL) {
    parent::init($view, $display, $options);

    // Initialize the query, which will provides us with an access to the
    // indexes used by this view, and thus to the entity types.
    if (is_null($this->view->query)) {
      $this->view->init_query();
    }

    $entity_types = $incompatibles = array();
    foreach ($this->view->query->getIndexes() as $id => $index) {
      $type = $index->getEntityType();
      $label = $index->label();

      if ($type) {
        $entity_types[$type][] = $label;
      }
      else {
        $incompatibles[$id] = $label;
      }
    }

    $this->indexes = array(
      'entity_types' => $entity_types,
      'incompatibles' => $incompatibles,
    );
  }

  /**
   * {@inheritdoc}
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['view_mode'] = array('default' => array());
    return $options;
  }

  /**
   * {@inheritdoc}
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $entity_info = entity_get_info();

    $options = array();
    foreach ($this->indexes['entity_types'] as $type => $indexes) {
      foreach ($entity_info[$type]['view modes'] as $view_mode => $view_mode_info) {
        $options[$type][$view_mode] = $view_mode_info['label'];
      }
    }

    $form['view_mode'] = array(
      '#tree' => TRUE,
    );

    foreach ($options as $entity_type => $modes) {
      if (count($modes) > 1) {
        $form['view_mode'][$entity_type] = array(
          '#type' => 'select',
          '#options' => $modes,
          '#title' => t('View mode for @entity_name entities', array(
            '@entity_name' => $entity_info[$entity_type]['label'],
          )),
          '#description' => format_plural(
            count($this->indexes['entity_types'][$entity_type]),
            'This view mode will be used to render entities from the following index: %indexes',
            'This view mode will be used to render entities from the following indexes: %indexes',
            array(
              '%indexes' => implode(', ', $this->indexes['entity_types'][$entity_type]),
            )
          ),
        );
        if (!empty($this->options['view_mode'][$entity_type])) {
          $form['view_mode'][$entity_type]['#default_value'] = $this->options['view_mode'][$entity_type];
        }
      }
      else {
        // For entity types that only have one view mode, there's no meaningful
        // choice to make, so just expose it to let the user know about it.
        $form['view_mode']["{$entity_type}__description"] = array(
          '#type' => 'item',
          '#title' => t('View mode for @entity_name entities', array(
            '@entity_name' => $entity_info[$entity_type]['label'],
          )),
          '#description' => reset($modes),
        );
        $form['view_mode'][$entity_type] = array(
          '#type' => 'value',
          '#value' => key($modes),
        );
      }
    }

    if (count($this->indexes['incompatibles'])) {
      $form['incompatibles'] = array(
        '#type' => 'item',
        '#title' => 'Incompatible indexes',
        '#description' => t("The following indexes are not based on entities, and can't be used with this row style: %indexes. Items from those indexes will be skipped during rendering.", array(
          '%indexes' => implode(', ', $this->indexes['incompatibles']),
        )),
      );
    }

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function pre_render($values) {
    if (!empty($values)) {
      list(, $this->wrappers) = $this->view->query->get_result_wrappers($values);
    }
  }

  /**
   * Returns a metadata wrapper for a returned row.
   *
   * @param object $values
   *   The values of the returned row.
   *
   * @return EntityMetadataWrapper|null
   *   A wrapper for that row, or NULL if the row doesn't represent an entity.
   */
  public function get_wrapper($values) {
    $index = $values->_entity_properties['search_api_multi_index'];
    if (isset($this->indexes['incompatibles'][$index])) {
      return NULL;
    }
    if (empty($this->wrappers[$this->view->row_index]->$index)) {
      return NULL;
    }
    return $this->wrappers[$this->view->row_index]->$index;
  }

  /**
   * {@inheritdoc}
   */
  public function render($values) {
    if ($wrapper = $this->get_wrapper($values)) {
      $render = $wrapper->view($this->options['view_mode'][$wrapper->type()]);
      return backdrop_render($render);
    }
  }
}
