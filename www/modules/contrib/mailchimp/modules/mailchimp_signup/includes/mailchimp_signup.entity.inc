<?php

/**
 * @file
 * Mailchimp Signup entity class
 */

class MailchimpSignup extends Entity {
  public
    $mcs_id,
    $name,
    $mc_lists,
    $mode,
    $title,
    $settings;

  /**
   * Override parent constructor to set the entity type.
   */
  public function __construct(array $values = array()) {
    // The Features module calls the entity controller with all values cast to
    // an array. The mergefields and its settings are returned by the Mailchimp
    // API as objects. When constructing the entity from the Features import,
    // cast the mergefields back to objects for a consistent data model.
    if (!empty($values['settings']['mergefields'])) {
      foreach ($values['settings']['mergefields'] as $tag => $mergefield) {
        if (is_array($mergefield)) {
          if (is_array($mergefield['options'])) {
            $mergefield['options'] = (object) $mergefield['options'];
          }
          $values['settings']['mergefields'][$tag] = (object) $mergefield;
        }
      }
    }

    parent::__construct($values, 'mailchimp_signup');
  }

  /**
   * Return a label for a signup form.
   */
  public function label() {
    return $this->title;
  }

  /**
   * Overrides Entity\Entity::uri().
   */
  public function uri() {
    return array(
      'path' => 'admin/config/services/mailchimp/signup/manage/' . $this->name,
      'options' => array(
        'entity_type' => $this->entityType(),
        'entity' => $this,
      ),
    );
  }

  /**
   * Implements EntityInterface::id().
   */
  public function id() {
    return $this->mcs_id;
  }

  /**
   * Implements EntityInterface::entityType().
   */
  public function entityType() {
    return 'mailchimp_signup';
  }

  /**
   * {@inheritdoc}
   */
  public function view($view_mode = 'full', $langcode = NULL, $page = NULL) {
    return entity_get_controller($this->entityType())->view(array($this), $view_mode, $langcode, $page);
  }


}
