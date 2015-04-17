<?php
/**
 * Validate whether an argument is a project node.
 */
class project_plugin_argument_validate extends views_plugin_argument_validate {
  function option_definition() {
    $options = parent::option_definition();
    $options['releases_enabled'] = array('default' => '1');
    return $options;
  }

  function options_form(&$form, &$form_state) {
    // Altering a view plugin definition/form/validate can be difficult, so we
    // are just including this option directly within project module.
    $form['releases_enabled'] = array(
      '#type' => 'radios',
      '#title' => t('Restrict to projects with'),
      '#options' => array(
        '1' => t('Releases enabled'),
        '0' => t('Releases disabled'),
        '' => t('Releases enabled or disabled'),
      ),
      '#default_value' => $this->options['releases_enabled'],
      '#description' => t('Only validate projects with releases enabled or disabled.'),
      '#access' => module_exists('project_release'),
    );
  }

  function validate_argument($argument) {
    $valid = FALSE;
    if (is_numeric($argument)) {
      $node = node_load($argument);
    }
    if (isset($node->type)) {
      $valid = in_array($node->type, project_project_node_types());
    }

    if ($valid && $this->options['releases_enabled'] !== '') {
      $valid = (intval($node->project['releases_enabled']) === intval($this->options['releases_enabled']));
    }

    return $valid;
  }
}
