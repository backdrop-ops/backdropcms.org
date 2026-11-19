<?php

// KG I need this - BestTestClass does not always autoload in local IDE
require_once __DIR__ . '/BaseTestClass.php';

use Civi\Test\HeadlessInterface;
use Civi\Test\HookInterface;
use Civi\Test\TransactionalInterface;

// KG
use Civi\Payment\System;


/**
 * FIXME - Add test description.
 *
 * Tips:
 *  - With HookInterface, you may implement CiviCRM hooks directly in the test class.
 *    Simply create corresponding functions (e.g. "hook_civicrm_post(...)" or similar).
 *  - With TransactionalInterface, any data changes made by setUp() or test****() functions will
 *    rollback automatically -- as long as you don't manipulate schema or truncate tables.
 *    If this test needs to manipulate schema or truncate tables, then either:
 *       a. Do all that using setupHeadless() and Civi\Test.
 *       b. Disable TransactionalInterface, and handle all setup/teardown yourself.
 *
 * @group headless
 */
class CRM_Iats_ContributioniATSTest extends BaseTestClass {

  public function setUpHeadless() {
    // Civi\Test has many helpers, like install(), uninstall(), sql(), and sqlFile().
    // See: https://github.com/civicrm/org.civicrm.testapalooza/blob/master/civi-test.md
    return \Civi\Test::headless()
      ->installMe(__DIR__)
      ->apply();
  }

  public function setUp(): void {
    $this->_apiversion = 3;
    parent::setUp();
  }

  public function tearDown(): void {
    parent::tearDown();
  }

  /**
   * Test a Credit Card Contribution - one time iATS Credit Card - TEST41 - Backend
   */
  public function testIATSCreditCardBackend(): void {

    $params = [
      'sequential' => 1,
      'first_name' => "Can",
      'last_name' => "ada",
      'contact_type' => "Individual",
    ];

    $individual = $this->callAPISuccess('contact', 'create', $params);

    // Need to create a Payment Processor - iATS Credit Card - TE4188
    $this->paymentProcessor = $this->iATSCCProcessorCreate();

    $processor = $this->paymentProcessor->getPaymentProcessor();
    $this->paymentProcessorID = $processor['id'];

    $contribution_params = [
      'soft_credit_contact_id' => [],
      'total_amount' => 1.00,
      'financial_type_id' => 1,
      'receive_date' => date('Y-m-d H:i:s'),
      'contact_id' => $individual['id'],
      'payment_instrument_id' => 1,
      'contribution_status_id' => 1,
      'credit_card_number' => 4222222222222220,
      'cvv2' => 123,
      'credit_card_exp_date' => [
        'M' => 12,
        'Y' => 2025,
      ],
      'credit_card_type' => 'Visa',
      'billing_first_name' => 'Karin',
      'billing_middle_name' => '',
      'billing_last_name' => 'G',
      'billing_street_address-5' => '39 St',
      'billing_city-5' => 'Calgary',
      'billing_state_province_id-5' => 1031,
      'billing_postal_code-5' => 10545,
      'billing_country_id-5' => 1228,
      'frequency_interval' => 1,
      'frequency_unit' => 'month',
      'installments' => '',
      'hidden_AdditionalDetail' => 1,
      'hidden_Premium' => 1,
      'receipt_date' => '',
      'cancel_date' => '',
      'payment_processor_id' => $this->paymentProcessorID,
      'currency' => 'CAD',
      'source' => 'iATS CC TEST88',
    ];

    $form = $this->getFormObject('CRM_Contribute_Form_Contribution', $contribution_params);
    $form->buildForm();
    $form->_mode = 'Live';
    $form->postProcess();

    $contribution = $this->callAPISuccessGetSingle('Contribution', [
      'contact_id' => $individual['id'],
      'contribution_status_id' => 'Completed',
    ]);
    $this->assertEquals('1.00', $contribution['total_amount']);
    $this->assertEquals(0, $contribution['non_deductible_amount']);

    // Make sure that we have a Transaction ID and that it contains a : (unique to iATS);
    $this->assertRegExp('/:/', $contribution['trxn_id']);

    // LineItems; Financial Tables;
  }

  /**
   * Create iATS - TEST41 CC Payment Processor.
   *
   * @param array $processorParams
   *
   * @return Instance of CC Payment Processor
   */
  public function iATSCCProcessorCreate($processorParams = []) {
    $paymentProcessorID = $this->processorCreateCC($processorParams);
    return System::singleton()->getById($paymentProcessorID);
  }

  /**
   * Create iATS - TEST41 SWIPE Payment Processor.
   *
   * @param array $processorParams
   *
   * @return Instance of SWIPE Payment Processor
   */
  public function iATSSWIPEProcessorCreate($processorParams = []) {
    $paymentProcessorID = $this->processorCreateSWIPE($processorParams);
    return System::singleton()->getById($paymentProcessorID);
  }

  /**
   * Create iATS Credit Card - TEST41 Payment Processor.
   * Payment Processor Type: 13 is iATS Payments Credit Card
   *
   * @return int
   *   Id Payment Processor
   */
  public function processorCreateCC($params = []) {
    $processorParams = [
      'domain_id' => 1,
      'name' => 'iATS Credit Card - TE4188',
      'payment_processor_type_id' => 'iATS Payments Credit Card',
      'financial_account_id' => 12,
      'is_test' => FALSE,
      'is_active' => 1,
      'user_name' => 'TE4188',
      'password' => 'abcde01',
      'url_site' => 'https://www.iatspayments.com/NetGate/ProcessLinkv2.asmx?WSDL',
      'url_recur' => 'https://www.iatspayments.com/NetGate/ProcessLinkv2.asmx?WSDL',
      'class_name' => 'Payment_iATSService',
      'is_recur' => 1,
      'sequential' => 1,
      'payment_type' => 1,
      'payment_instrument_id' => 1,
    ];
    $processorParams = array_merge($processorParams, $params);
    $processor = $this->callAPISuccess('PaymentProcessor', 'create', $processorParams);
    return $processor['id'];
  }

  /**
   * Create iATS SWIPE - TEST41 Payment Processor.
   * Payment Processor Type: 15 is iATS Payments SWIPE
   *
   * @return int
   *   Id Payment Processor
   */
  public function processorCreateSWIPE($params = []) {
    $processorParams = [
      'domain_id' => 1,
      'name' => 'iATS Credit Card - TE4188',
      'payment_processor_type_id' => 'iATS Payments SWIPE',
      'financial_account_id' => 12,
      'is_test' => FALSE,
      'is_active' => 1,
      'user_name' => 'TE4188',
      'password' => 'abcde01',
      'url_site' => 'https://www.iatspayments.com/NetGate/ProcessLinkv2.asmx?WSDL',
      'url_recur' => 'https://www.iatspayments.com/NetGate/ProcessLinkv2.asmx?WSDL',
      'class_name' => 'Payment_iATSServiceSWIPE',
      'is_recur' => 1,
      'sequential' => 1,
      'payment_type' => 1,
      'payment_instrument_id' => 1,
    ];
    $processorParams = array_merge($processorParams, $params);
    $processor = $this->callAPISuccess('PaymentProcessor', 'create', $processorParams);
    return $processor['id'];
  }
}
