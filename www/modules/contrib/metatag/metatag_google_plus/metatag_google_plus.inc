<?php
/**
 * @file
 * Metatag controller for Google+.
 */

/**
 * Schema meta tag controller.
 *
 * This extends BackdropTextMetaTag as we need to alter variables in
 * template_preprocess_html() rather output a normal meta tag.
 *
 * This controller is little different from BackdropTitleMetaTag and same value in
 * the itemtype instead of title. So in the MYTHEME_preprocess_html(),
 * $variable['itemtype'] will be provided.
 */
class BackdropSchemaMetaTag extends BackdropTextMetaTag {

  public function getElement(array $options = array()) {
    $element = array();
    $value = $this->getValue($options);
    $element['#attached']['metatag_set_preprocess_variable'][] = array('body', 'itemtype', $value);
    return $element;
  }
}
