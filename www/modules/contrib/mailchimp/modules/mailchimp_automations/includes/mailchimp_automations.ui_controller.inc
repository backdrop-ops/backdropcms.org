<?php

/**
 * @file
 * Class file for Mailchimp Automation UI Controller.
 */

/**
 * Override EntityDefaultUIController to customize our menu items.
 */
class MailChimpAutomationUIController extends EntityDefaultUIController {

  /**
   * Overrides parent::hook_menu().
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage MailChimp Automation entity settings.';
    $items[$this->path]['type'] = MENU_LOCAL_TASK;
    $items[$this->path]['access callback'] = 'mailchimp_automations_entity_access';
    $items[$this->path]['title'] = "Automations";
    $items[$this->path]['weight'] = 10;
    return $items;
  }

}
