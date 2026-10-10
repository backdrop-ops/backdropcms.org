<?php

/**
 * @file
 * Holds the Entity Plus module's theme functions.
 */

/**
 * Returns HTML for an entity property.
 *
 * This is the default theme implementation to display the value of a property.
 * This function can be overridden with varying levels of specificity. For
 * example, for a property named 'title' displayed on the 'article' bundle,
 * any of the following functions will override this default implementation.
 * The first of these functions that exists is used:
 * - THEMENAME_entity_plus_property__article__title()
 * - THEMENAME_entity_plus_property__title()
 * - THEMENAME_entity_plus_property()
 *
 *  Including
 *  'extra fields controller class' => 'EntityDefaultExtraFieldsController'
 *  within hook_entity_info for a given custom entity will call this theme to render all its properties.
 *
 * @param array $variables
 *   An associative array containing:
 *   - label_hidden: A boolean indicating to show or hide the property label.
 *   - label: The label for the property
 *   - property_name: The name of the property
 *   - content: The rendered property value.
 *   - title_attributes_array: An array containing the attributes for the title.
 *   - content_attributes_array: An array containing the attributes for the content's
 *     div.
 *   - attributes_array: A string containing the attributes for the top-level wrapping div.
 *
 * @ingroup themeable
 */
function theme_entity_plus_property($variables) {
  $output = '';

  $attributes = empty($variables['attributes_array']) ? '' : backdrop_attributes($variables['attributes_array']);
  $title_attributes = empty($variables['title_attributes_array']) ? '' : backdrop_attributes($variables['title_attributes_array']);
  $content_attributes = empty($variables['content_attributes_array']) ? '' : backdrop_attributes($variables['content_attributes_array']);

  // Render the label, if it's not hidden.
  if (!$variables['label_hidden']) {
    $output .= '<div' . $title_attributes . '>' . $variables['label'] . ':&nbsp;</div>';
  }

  // Render the content.
  $content_suffix = '';
  if (!$variables['label_hidden'] || $content_attributes) {
    $output .= '<div' . $content_attributes . '>';
    $content_suffix = '</div>';
  }
  $output .= $variables['content'] . $content_suffix;

  // Render the top-level DIV.
  return '<div' . $attributes . '>' . $output . '</div>';
}


/**
 * Theme preprocess function for theme_entity_plus_property().
 *
 * @see theme_entity_plus_property()
 */
function template_preprocess_entity_plus_property(&$variables, $hook) {
  $element = $variables['elements'];

  $variables += array(
    'theme_hook_suggestions' => array(),
    'attributes_array' => array(),
  );
  // Generate variables from element properties.
  foreach (array('label_hidden', 'label', 'property_name') as $name) {
    if (isset($element['#' . $name])) {
      $variables[$name] = check_plain($element['#' . $name]);
    }
  }

  if (isset($element['#label_display'])) {
    $variables['label_hidden'] = ($element['#label_display'] == 'hidden');
  }

  $variables['title_attributes_array']['class'][] = 'entity-property-label';
  $variables['content_attributes_array']['class'][] = 'entity-property-item';
  $variables['attributes_array'] = array_merge($variables['attributes_array'], isset($element['#attributes']) ? $element['#attributes'] : array());

  $variables['property_name_css'] = strtr($element['#property_name'], '_', '-');
  $variables['attributes_array']['class'][] = 'entity-property';
  $variables['attributes_array']['class'][] = 'entity-property-' . $variables['property_name_css'];

  if (isset($element['#label_display']) && $element['#label_display'] == 'inline') {
    $variables['attributes_array']['class'][] = 'entity-property-' . $element['#label_display'];
    $variables['attributes_array']['class'][]  = 'clearfix';
  }

  // Add specific suggestions that can override the default implementation.
  $variables['theme_hook_suggestions'] += array(
    'entity_plus_property__' . $element['#property_name'],
    'entity_plus_property__' . $element['#entity_type'] . '__' . $element['#property_name'],
  );
  // Populate the content with sensible defaults.
  if (!isset($element['#content'])) {
    $variables['content'] = entity_plus_property_default_render_value_by_type($element['#entity_plus_wrapped']->{$element['#property_name']});
  }
  else {
    $variables['content'] = $element['#content'];
  }
}

/**
 * Renders a property using simple defaults based upon the property type.
 *
 * @return string
 *   The rendered property value.
 */
function entity_plus_property_default_render_value_by_type(EntityMetadataWrapper $property) {
  // If there is an options list or entity label, render that by default.
  if ($label = $property->label()) {
    if ($property instanceof EntityBackdropWrapper && $uri = entity_uri($property->type(), $property->value())) {
      return l($label, $uri['path'], $uri['options']);
    }
    else {
      return check_plain($label);
    }
  }
  switch ($property->type()) {
    case 'boolean':
      return $property->value() ? t('yes') : t('no');

    default:
      return check_plain($property->value());
  }
}

/**
 * Process variables for entity_plus.tpl.php.
 */
function template_preprocess_entity_plus(&$variables) {
  $variables['view_mode'] = $variables['elements']['#view_mode'];
  // Backwards compatibility is included below for potential contrib or custom
  // code that adapted to the non-standard `entity_plus_type' key.
  // See https://github.com/backdrop-contrib/entity_plus/issues/44.
  $entity_type = !empty($variables['elements']['#entity_type']) ? $variables['elements']['#entity_type'] : $variables['elements']['#entity_plus_type'];
  $variables['entity_type'] = $entity_type;
  $entity = $variables['elements']['#entity'];
  $variables[$entity_type] = $entity;
  $info = entity_get_info($entity_type);

  $variables['title'] = check_plain(entity_label($entity_type, $entity));

  $uri = entity_uri($entity_type, $entity);
  $variables['url'] = $uri && !empty($uri['path']) ? url($uri['path'], $uri['options']) : FALSE;

  if (isset($variables['elements']['#page'])) {
    // If set by the caller, respect the page property.
    $variables['page'] = $variables['elements']['#page'];
  }
  else {
    // Else, try to automatically detect it.
    $variables['page'] = $uri && !empty($uri['path']) && $uri['path'] == $_GET['q'];
  }

  // Helpful $content variable for templates.
  $variables['content'] = array();
  foreach (element_children($variables['elements']) as $key) {
    $variables['content'][$key] = $variables['elements'][$key];
  }

  if (!empty($info['fieldable'])) {
    // Make the field variables available with the appropriate language.
    field_attach_preprocess($entity_type, $entity, $variables['content'], $variables);
  }
  list(, , $bundle) = entity_extract_ids($entity_type, $entity);

  // Set 'classes_array' as an array if it isn't set.
  if (!isset($variables['classes_array'])) {
    $variables['classes_array'] = array();
  };
  // Merge the two class arrays.
  $css_classes = array_merge($variables['classes'], $variables['classes_array']);
  $css_classes[] = backdrop_html_class('entity-' . $entity_type);
  $css_classes[] = backdrop_html_class($entity_type . '-' . $bundle);

  // Add css classes to the standard 'classes' array used by Backdrop entities.
  // For backward compatibility, add the same classes to 'classes_array'. 
  $variables['classes'] = $variables['classes_array'] = $css_classes;

  // Add RDF type and about URI.
  if (module_exists('rdf')) {
    $variables['attributes_array']['about'] = empty($uri['path']) ? NULL : url($uri['path']);
    $variables['attributes_array']['typeof'] = empty($entity->rdf_mapping['rdftype']) ? NULL : $entity->rdf_mapping['rdftype'];
  }

  // Add suggestions.
  $variables['theme_hook_suggestions'][] = $entity_type;
  $variables['theme_hook_suggestions'][] = $entity_type . '__' . $bundle;
  $variables['theme_hook_suggestions'][] = $entity_type . '__' . $bundle . '__' . $variables['view_mode'];
  if ($id = entity_plus_id($entity_type, $entity)) {
    $variables['theme_hook_suggestions'][] = $entity_type . '__' . $id;
  }
}

/**
 * Themes the exportable status of an entity.
 */
function theme_entity_plus_status($variables) {
  $status = $variables['status'];
  $html = $variables['html'];
  if (($status & ENTITY_PLUS_FIXED) == ENTITY_PLUS_FIXED) {
    $label = t('Fixed');
    $help = t('The configuration is fixed and cannot be changed.');
    return $html ? "<span class='entity-status-fixed' title='$help'>" . $label . "</span>" : $label;
  }
  elseif (($status & ENTITY_PLUS_OVERRIDDEN) == ENTITY_PLUS_OVERRIDDEN) {
    $label = t('Overridden');
    $help = t('This configuration is provided by a module, but has been changed.');
    return $html ? "<span class='entity-status-overridden' title='$help'>" . $label . "</span>" : $label;
  }
  elseif ($status & ENTITY_PLUS_IN_CODE) {
    $label = t('Default');
    $help = t('A module provides this configuration.');
    return $html ? "<span class='entity-status-default' title='$help'>" . $label . "</span>" : $label;
  }
  elseif ($status & ENTITY_PLUS_CUSTOM) {
    $label = t('Custom');
    $help = t('A custom configuration by a user.');
    return $html ? "<span class='entity-status-custom' title='$help'>" . $label . "</span>" : $label;
  }
}
