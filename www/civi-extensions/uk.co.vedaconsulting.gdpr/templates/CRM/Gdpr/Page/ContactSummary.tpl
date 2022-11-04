<div id="last_acceptance_date" class="crm-summary-row" style="display: none;">
  <div class="crm-label">{ts}GDPR Status{/ts}</div>
  <div class="crm-content crm-contact-gdpr_status">
		{if $lastAcceptanceDate}
			Communication preferences submitted on {$lastAcceptanceDate}
		{/if}
	</div>
</div>
{literal}
<script type="text/javascript">
	CRM.$(function($){
		if ($('#crm-communication-pref-content').length !== 0) {
			$('#last_acceptance_date').prependTo('#crm-communication-pref-content .crm-inline-block-content');
			$('#last_acceptance_date').show();
		}
	});
</script>
{/literal}
