<?php
/**
 * @file
 * IP Demerit class and interface.
 */

/**
 * IpDemerit extends Entity.
 *
 * We need to create an Entity in order to use Views Bulk Actions on demerits.
 */
class IpDemerit extends Entity {

  /**
   * Implements EntityInterface::id().
   */
  public function id() {
    return isset($this->id) ? $this->id : NULL;
  }

  /**
   * Implements EntityInterface::entityType().
   */
  public function entityType() {
    return 'ip_demerit';
  }

  /**
   * Implements EntityInterface::bundle().
   */
  public function bundle() {}

  /**
   * Implements EntityInterface::label().
   */
  public function label() {}

  /**
   * Implements EntityInterface::uri().
   */
  public function uri() {}
}
