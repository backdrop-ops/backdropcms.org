<?php
use CRM_Airmail_Utils as E;

class CRM_Airmail_Page_Webhook extends CRM_Core_Page {

  public function run() {

    $settings = E::getSettings();
    if (!empty($settings['secretcode']) && $settings['secretcode'] !== ($_GET['secretcode'] ?? '')) {
      $this->invalidMessage("Missing or invalid Secret Code");
    }

    $backend = E::getBackend();
    // Check that this is a real backend.
    if (!$backend || !in_array('CRM_Airmail_Backend', class_implements($backend))) {
      $this->invalidMessage("Missing or invalid Airmail Backend.");
    }

    // Process the input.
    $events = $backend->processInput(file_get_contents('php://input'));

    // Make sure the processed input exists and is valid according to the backend.
    if (!$events || !$backend->validateMessages($events)) {
      $this->invalidMessage("Request body is empty or invalid.");
    }

    // Process the message(s) in the processed input
    $backend->processMessages($events);

    CRM_Utils_System::civiExit();
  }

  /**
   * What should happen if we want to reject the message without processing it.
   */
  protected function invalidMessage($errorMessage) {
    Civi::log()->error($errorMessage);

    CRM_Utils_System::sendResponse(
      new \GuzzleHttp\Psr7\Response(400)
    );

    CRM_Utils_System::civiExit();
  }

}
