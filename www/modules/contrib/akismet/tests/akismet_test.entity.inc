<?php

/**
 * @file
 * Entity controller and class for comments.
 */

/**
 * Defines the comment entity class.
 */
class AkismetTest extends Entity {

  /**
   * The ID.
   *
   * @var integer
   */
  public $mid;

  /**
   * The title.
   *
   * @var string
   */
  public $label;

  /**
   * Implements EntityInterface::id().
   */
  public function id() {
    return $this->mid;
  }

  /**
   * Implements EntityInterface::entityType().
   */
  public function entityType() {
    return 'akismet_test';
  }

  /**
   * Implements EntityInterface::bundle().
   */
  public function bundle() {
    return $this->entityType();
  }

  /**
   * Implements EntityInterface::label().
   */
  public function label() {
    return $this->label;
  }

  /**
   * Implements EntityInterface::uri().
   */
  public function uri() {
    return array(
      'path' => 'akismet-test/form/' . $this->mid,
    );
  }
}
