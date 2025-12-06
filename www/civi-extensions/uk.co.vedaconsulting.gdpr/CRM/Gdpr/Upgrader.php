<?php
use CRM_Gdpr_ExtensionUtil as E;

/**
 * Collection of upgrade steps.
 */
class CRM_Gdpr_Upgrader extends CRM_Extension_Upgrader_Base {

  // By convention, functions that look like "function upgrade_NNNN()" are
  // upgrade tasks. They are executed in order (like Drupal's hook_update_N).

  /**
   * Example: Run an external SQL script when the module is installed.
   */
  public function install() {
    // Create 'GDPR Cancelled' membership status
    $this->createGDPRCancelledMembershipStatus();
  }

  /**
   * Example: Work with entities usually not available during the install step.
   *
   * This method can be used for any post-install tasks. For example, if a step
   * of your installation depends on accessing an entity that is itself
   * created during the installation (e.g., a setting or a managed entity), do
   * so here to avoid order of operation problems.
   *
   **/
  public function postInstall() {
    $this->executeCustomDataFile('xml/CustomData_v1.xml');
    $this->executeCustomDataFile('xml/CustomGroupData.xml');
  }

  /**
   * Example: Run an external SQL script when the module is uninstalled.
   */
  public function uninstall() {
    // Delete 'GDPR Cancelled' membership status
    $result = CRM_Gdpr_Utils::CiviCRMAPIWrapper('MembershipStatus', 'get', [
      'sequential' => 1,
      'return' => ["id"],
      'name' => "GDPR_Cancelled",
      'api.MembershipStatus.delete' => [
        'id' => "\$value.id",
      ],
    ]);

    // Delete 'Contacts without any activity for a period' custom search
    $result = CRM_Gdpr_Utils::CiviCRMAPIWrapper('CustomSearch', 'get', [
      'sequential' => 1,
      'return' => ["id"],
      'name' => "CRM_Gdpr_Form_Search_ActivityContact",
      'api.CustomSearch.delete' => [
        'id' => "\$value.id",
      ],
    ]);

    // Delete 'Search Group Subscription by Date Range' custom search
    $result = CRM_Gdpr_Utils::CiviCRMAPIWrapper('CustomSearch', 'get', [
      'sequential' => 1,
      'return' => ["id"],
      'name' => "CRM_Gdpr_Form_Search_GroupcontactDetails",
      'api.CustomSearch.delete' => [
        'id' => "\$value.id",
      ],
    ]);

    // Delete custom data.
    $result = civicrm_api3('CustomGroup', 'get', [
      'name' => ['IN' => [
        'SLA_Acceptance',
        'Event_terms_and_conditions',
        'Event_terms_and_conditions_acceptance',
        'Contribution_Page_terms_and_conditions',
        'Contribution_terms_and_conditions_acceptance',
      ]],
      'return' => ['id']
    ]);
    $group_ids = array_keys($result['values'] ?? []);
    if ($group_ids) {
      // Found one or more of our custom groups.
      // Lookup fields for these and delete those first.
      $fields = civicrm_api3('CustomField', 'get', [
        'custom_group_id' => ['IN' => $group_ids],
        'return'          => ['id'],
      ]);
      foreach (array_keys($fields['values'] ?? []) as $field_id) {
        civicrm_api3('CustomField', 'delete', ['id' => $field_id]);
      }

      // Now delete the groups themselves.
      foreach ($group_ids as $group_id) {
        civicrm_api3('CustomGroup', 'delete', ['id' => $group_id]);
      }
    }
  }

  /**
   * Example: Run a simple query when a module is enabled.
   */
  public function enable() {
    // Enable 'GDPR Cancelled' membership status
    $result = CRM_Gdpr_Utils::CiviCRMAPIWrapper('MembershipStatus', 'get', [
      'sequential' => 1,
      'return' => ["id"],
      'name' => "GDPR_Cancelled",
      'api.MembershipStatus.create' => [
        'id' => "\$value.id",
        'is_active' => 1,
      ],
    ]);

    $this->addMsgTemplateGDPR();
  }

  /**
   * Example: Run a simple query when a module is disabled.
   */
  public function disable() {
    // Disable 'GDPR Cancelled' membership status
    $result = CRM_Gdpr_Utils::CiviCRMAPIWrapper('MembershipStatus', 'get', [
      'sequential' => 1,
      'return' => ["id"],
      'name' => "GDPR_Cancelled",
      'api.MembershipStatus.create' => [
        'id' => "\$value.id",
        'is_active' => 0,
      ],
    ]);
  }

  /**
   * Perform upgrade to version 1.1
   *
   * @return TRUE on success
   * @throws Exception
   */
  public function upgrade_1100() {
    $this->log('Applying update 1100');
    // Create 'Contacts without any activity for a period' custom search by API
    CRM_Gdpr_Utils::CiviCRMAPIWrapper('CustomSearch', 'create', [
      'sequential' => 1,
      'option_group_id' => "custom_search",
      'name' => "CRM_Gdpr_Form_Search_ActivityContact",
      'is_active' => 1,
      'label' => "CRM_Gdpr_Form_Search_ActivityContact",
      'description' => E::ts("Contacts without any activity for a period"),
    ]);
    return TRUE;
  }

  /**
   * Perform upgrade to version 1.2
   *
   * @return TRUE on success
   * @throws Exception
   */
  public function upgrade_1200() {
    $this->log('Applying update 1200');
    // Create 'GDPR Cancelled' membership status
    $this->createGDPRCancelledMembershipStatus();
    return TRUE;
  }

  /**
   * Perform upgrade to version 1.2.0.1
   *
   * @return TRUE on success
   * @throws Exception
   */
  public function upgrade_1201() {
    $this->ctx->log->info('Applying update 1.2.0.1');
    CRM_Core_ManagedEntities::singleton(TRUE)->reconcile();
    $this->executeCustomDataFile('xml/CustomGroupData.xml');
    return TRUE;
  }

  /**
   * Perform upgrade to version 1.2.0.2
   *
   * @return TRUE on success
   * @throws Exception
   */
  public function upgrade_1202() {
    $this->ctx->log->info('Applying update 1.2.0.2');
    // Change labels for custom data.
    $sql_file = 'sql/alterCustomDataLabels.sql';
    $this->executeSqlFile($sql_file);
    return TRUE;
  }

  /**
   * Perform upgrade to version 1.2.0.3
   *
   * @return TRUE on success
   * @throws Exception
   */
  public function upgrade_1203() {
    $this->ctx->log->info('Applying update 1.2.0.3');
    // Reconcile managed entity for upgrade v2.0 -> v2.2.1
    CRM_Core_ManagedEntities::singleton(TRUE)->reconcileEnabledModules();
    $this->executeCustomDataFile('xml/CustomGroupData.xml');
    return TRUE;
  }

  /**
   * Perform upgrade to version 1.2.0.4
   *
   * @return TRUE on success
   * @throws Exception
   */
  public function upgrade_1204() {
    $this->ctx->log->info('Applying update 1.2.0.4, to create activity type forget me');
    $this->executeCustomDataFile('xml/CustomGroupData.xml');
    return TRUE;
  }

  public function upgrade_1205() {
    $this->ctx->log->info('Adding Update Communication Preferences MessageTemplate');
    $this->addMsgTemplateGDPR();
    return TRUE;
  }

  /**
   * Create 'GDPR Cancelled' membership status
   */
  private function createGDPRCancelledMembershipStatus() {
    $result = CRM_Gdpr_Utils::CiviCRMAPIWrapper('MembershipStatus', 'get', [
      'name' => "GDPR_Cancelled",
    ]);
    if ($result['count']) {
      return ;
    }

    // Get max weight for membership status
    $result = CRM_Gdpr_Utils::CiviCRMAPIWrapper('MembershipStatus', 'get', [
      'sequential' => 1,
      'return' => ["weight"],
      'options' => ['sort' => "weight DESC", 'limit' => 1],
    ]);
    $weight = $result['values'][0]['weight'] + 1;

    // Create 'GDPR Cancelled' membership status
    CRM_Gdpr_Utils::CiviCRMAPIWrapper('MembershipStatus', 'create', [
      'name' => "GDPR_Cancelled",
      'label' => E::ts("GDPR Cancelled"),
      'is_admin' => 1, // Is Admin Only
      'is_active' => 1,
      'is_reserved' => 1, // Is reserved, so that users cannot delete it
      'is_current_member' => 0,
      'weight' => $weight,
    ]);
  }

  private function log($message) {
    if (is_object($this->ctx) && method_exists($this->ctx, 'info')) {
      $this->ctx->log->info($message);
    }
  }

  private function addMsgTemplateGDPR() {
    // Create msg_tpl_workflow_gdpr optiongroup
    $existing = civicrm_api3('OptionGroup', 'get', [
      'name' => "msg_tpl_workflow_gdpr",
    ]);
    if (empty($existing['count'])) {
      $optionGroup = civicrm_api3('OptionGroup', 'create', [
        'name' => "msg_tpl_workflow_gdpr",
        'title' => E::ts("Message Template Workflow for GDPR"),
        'is_reserved' => 1,
        'is_active' => 1,
      ]);
    }
    else {
      $optionGroup = CRM_Utils_Array::first($existing['values']);
    }
    // Create msg_tpl_workflow_gdpr optionvalue
    $existing = civicrm_api3('OptionValue', 'get', [
      'name' => "gdpr_update_preferences",
    ]);
    if (empty($existing['count'])) {
      $optionValue = civicrm_api3('OptionValue', 'create', [
        'name' => "gdpr_update_preferences",
        'title' => E::ts("Update Communication Preferences"),
        'is_reserved' => 0,
        'is_active' => 1,
        'option_group_id' => $optionGroup['id'],
      ]);
    }
    else {
      $optionValue = CRM_Utils_Array::first($existing['values']);
    }

    // Create msg template
    $msgTemplate = civicrm_api3('MessageTemplate', 'get', [
      'workflow_id' => $optionValue['id'],
    ]);
    if (empty($msgTemplate['count'])) {
      $msgTemplateParams = [
        'msg_title' => 'Update Communication Preferences',
        'msg_subject' => '{ts}You\'ve updated your communication preferences{/ts}',
        'msg_text' => '{ts 1=$display_name}Dear %1{/ts},

{if $confirm_email_text}{$confirm_email_text}
{/if}

{ts}Your communication preferences have been updated.{/ts}',
        'msg_html' => '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title></title>
<p>{capture assign=headerStyle}colspan=&quot;2&quot; style=&quot;text-align: left; padding: 4px; border-bottom: 1px solid #999; background-color: #eee;&quot;{/capture} {capture assign=labelStyle }style=&quot;padding: 4px; border-bottom: 1px solid #999; background-color: #f7f7f7;&quot;{/capture} {capture assign=valueStyle }style=&quot;padding: 4px; border-bottom: 1px solid #999;&quot;{/capture}</p>

<center>
<table border="0" cellpadding="0" cellspacing="0" id="crm-event_receipt" style="font-family: Arial, Verdana, sans-serif; text-align: left;" width="620"><!-- BEGIN HEADER --><!-- You can add table row(s) here with logo or other header elements --><!-- END HEADER --><!-- BEGIN CONTENT -->
	<tbody>
		<tr>
			<td>
			<p>{ts 1=$display_name}Dear %1{/ts},</p>
			{if $confirm_email_text}<p>{$confirm_email_text|htmlize}</p>{/if}
			<p>{ts}Your communication preferences have been updated.{/ts}</p>
			</td>
		</tr>
	</tbody>
</table>
</center>',
        'is_active' => 1,
        'workflow_id' => $optionValue['id'],
        'is_default' => 1,
        'is_reserved' => 0,
        'is_sms' => 0,
      ];
      // First we create the "current"
      civicrm_api3('MessageTemplate', 'create', $msgTemplateParams);
      // Now we create the "master"
      $msgTemplateParams['is_default'] = 0;
      $msgTemplateParams['is_reserved'] = 1;
      civicrm_api3('MessageTemplate', 'create', $msgTemplateParams);
    }
  }

  public function upgrade_1206() {
    $this->ctx->log->info('Updating Contribution Page custom fields');
    // Change Contribution Page custom group style to Tab to avoid showing twice
    \Civi\Api4\CustomGroup::update(FALSE)
      ->addWhere('name', '=', 'Contribution_Page_terms_and_conditions')
      ->addValue('style', 'Tab')
      ->execute();
    return TRUE;
  }

}
