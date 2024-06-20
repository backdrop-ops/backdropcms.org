{crmScope extensionKey='batchupdateactivitystatus'}
    <div class="crm-submit-buttons">
        {include file="CRM/common/formButtons.tpl" location="top"}
    </div>

    <h3>{$title}</h3>
    {if ($status)}
        <div class="messages status no-popup">
            {$status}
        </div>
    {/if}
    <div class="crm-block crm-form-block crm-searchactiondesigner-configuration-block">
        {if ($help_text)}
            <div class="help">{$help_text}</div>
        {/if}
        {if isset($fields)}
            {foreach from=$fields item=field}
                {include file=$field.template field=$field}
            {/foreach}
        {/if}
    </div>

    <div class="crm-submit-buttons">
        {include file="CRM/common/formButtons.tpl" location="bottom"}
    </div>
{/crmScope}
