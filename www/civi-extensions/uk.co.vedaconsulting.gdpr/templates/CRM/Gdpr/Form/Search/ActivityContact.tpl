<div id="help">
    {ts}Contacts who have not had any activity of the below types for {$settings.activity_period} days{/ts}
    <br />
    <strong>{$gdprActTypes}</strong>
</div>

{include file="CRM/Contact/Form/Search/Custom.tpl"}

{literal}
<script type="text/javascript">
	CRM.$(function($){
		$('form#Custom').removeClass('crm-ajax-selection-form');
	});
</script>
{/literal}