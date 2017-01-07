<?php

/**
 * This base plugin allows "link rel" tags to be further customized.
 */

namespace Drupal\metatag\Plugin\metatag\Tag;

abstract class LinkRelBase extends MetaNameBase {
  /**
   * Display the meta tag.
   */
  public function output() {
    if (empty($this->value)) {
      // If there is no value, we don't want a tag output.
      $element = '';
    }
    else {
      $element = [
        '#tag' => 'link',
        '#attributes' => [
          'rel' => $this->name,
          'href' => $this->value(),
        ]
      ];
    }

    return $element;
  }
}
