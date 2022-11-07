<?php

/**
 * @file
 * Helper class to test SmtpMailSystem.
 */

/**
 * Helper class to test SmtpMailSystem.
 */
class SmtpMailSystemWrapper extends SmtpMailSystem {

  /**
   * Expose public method to directly test _get_components().
   */
  public function getComponents($input) {
    return $this->_get_components($input);
  }

}
