<div class="tc-agreement-text">
{$agreement_text}
</div>
{if $tc_url}
<div class="tc-link">
<a href="{$tc_url}" target="blank">{$tc_link_label}</a>
</div>
{/if}
<div class="tc-agreement-checkbox">
  {$form.accept_tc.html} 
  <span class="label">{$form.accept_tc.label}</span>
  </div>
<div class="crm-submit-buttons">
{include file="CRM/common/formButtons.tpl" location="bottom"}
</div>
