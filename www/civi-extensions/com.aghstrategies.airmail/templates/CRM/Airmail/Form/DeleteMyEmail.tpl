{if $showForm}
  <p>{ts}You have two options{/ts}</p>
  <ol>
    <li>
      <p><strong>{ts}Opt-out of all bulk mail{/ts}</strong></p>
      <p>{ts}This will prevent you receiving any mail sent to subscribers of mailing lists, e.g. newsletters, membership subscriptions etc.{/ts}</p>
    </li>
    <li>
      <p><strong>{ts 1=$email}Opt-out and delete your email (%1){/ts}</strong></p>
      <p>{ts}This will do as (1) but will also mean that we will not be able to email that address for thank yous/receipts, confirmations, payment notifications, event tickets, membership information or other such administrative notifications. As a result you might miss something important.{/ts}</p>
    </li>
  </ol>

  <p>{$form.optoutoptions.label}</p>
  <div>{$form.optoutoptions.html}</div>

  <div class="crm-submit-buttons">
    {include file="CRM/common/formButtons.tpl" location="bottom"}
  </div>
{/if}
