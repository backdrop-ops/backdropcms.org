<?php

/**
 * Interface for SMTP services
 */
interface CRM_Airmail_Backend {

  /**
   * Process the raw input (such as JSON) into something we can handle.
   * @param string $input
   *   The raw PHP input.
   * @return mixed
   *   The value(s) to evaluate.
   */
  public function processInput($input);

  /**
   * Take a webhook message and validate whether it should be processed or
   * return a 404.
   *
   * @param object|array $events
   *   The JSON-decoded message.
   * @return bool
   *   Whether it looks valid.
   */
  public function validateMessages($events);

  public function processMessages($events);

  /**
   * Called by hook_civicrm_alterMailParams
   *
   * @param array $params
   *   The mailing params
   * @param string $context
   *   The mailing context.
   */
  public function alterMailParams(&$params, $context);

}
