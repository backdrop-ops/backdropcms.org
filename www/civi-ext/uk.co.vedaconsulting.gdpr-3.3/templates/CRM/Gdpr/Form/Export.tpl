{crmScope extensionKey='uk.co.vedaconsulting.gdpr'}
<div class="crm-block crm-form-block crm-gdpr-comms-prefs-form-block">
	<div id="help">
		<strong>NOTE:</strong> The export process may take long, depending on the data size which is exported.
	</div>
  <div class="crm-section">
	  <div class="label">{$form.export_entities.label}</div>
	  <div class="content">{$form.export_entities.html}</div>
	  <div class="clear"></div>
	</div>
	<div class="crm-section">
	  <div class="label">{$form.export_format.label}</div>
	  <div class="content">{$form.export_format.html}</div>
	  <div class="clear"></div>
	</div>
</div>

{* FOOTER *}
<div class="crm-submit-buttons">
{include file="CRM/common/formButtons.tpl" location="bottom"}
</div>
{/crmScope}
