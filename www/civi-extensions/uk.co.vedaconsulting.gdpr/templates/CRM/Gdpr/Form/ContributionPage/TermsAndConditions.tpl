{* HEADER *}
<div id="form" class="crm-form-block crm-tc-manage-block">
  <div class="crm-submit-buttons">
  {include file="CRM/common/formButtons.tpl" location="top"}
  </div>
  {foreach from=$elementNames item=elementName}
   {if $elementName == $terms_conditions_file_element_name}

   {elseif $elementName == $terms_conditions_link_element_name}
      <div class="crm-section">
        <div class="label">{$form.$terms_conditions_file_element_name.label}</div>
        <div class="content">{$form.$terms_conditions_file_element_name.html}
        {if $terms_conditions_current.url}
          <div class="terms-conditions-file-url">{ts}Current: <a href="{$terms_conditions_current.url}">{$terms_conditions_current.label}{/ts}</a>
        {/if}

        </div>
        <div class="clear"></div>
      </div>
   {else}
      <div class="crm-section">
        <div class="label">{$form.$elementName.label}</div>
        <div class="content">{$form.$elementName.html}
        <div class="description">{$descriptions.$elementName}</div>
        </div>
        <div class="clear"></div>
      </div>
    {/if}
  {/foreach}

  {* FOOTER *}
  <div class="crm-submit-buttons">
  {include file="CRM/common/formButtons.tpl" location="bottom"}
  </div>
</div>
{literal}
<script>
(function($) {
  var save = $('.crm-tc-manage-block .crm-button_qf_TermsAndConditions_upload_done,.crm-tc-manage-block .crm-button_qf_TermsAndConditions_submit_savenext').hide();
}(cj))
</script>
{/literal}
