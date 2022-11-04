{crmScope extensionKey='uk.co.vedaconsulting.gdpr'}
{if $crmPermissions->check('forget contact')}
<div class="action-link">
  {capture assign=forgetMeURL}{crmURL p="civicrm/gdpr/forgetme" q="reset=1&cid=`$contactId`"}{/capture}
  <a href="{$forgetMeURL}" class="button small-popup"><span><i class="crm-i fa-chain-broken"></i> {ts}Forget Me{/ts}</span></a>
  {capture assign=exportURL}{crmURL p="civicrm/gdpr/export" q="reset=1&cid=`$contactId`"}{/capture}
  <a href="{$exportURL}" class="button no-popup"><span><i class="crm-i fa-check"></i> {ts}Export{/ts}</span></a>
  <br/><br/>
</div>
{/if}

<h3>{ts}Summary{/ts}</h3>

<div class="crm-block crm-form-block">
    <div>
      <table class="selector row-highlight" id="SummaryTable">
        <thead class="sticky">
        <tr>
         <th scope="col">{ts}Subject{/ts}</th>
         <th scope="col">{ts}Status{/ts}</th>
         <th scope="col">{ts}Date{/ts}</th>
        </tr>
        </thead>
        <tbody>
        {foreach from=$summary item=summaryItem}
        <tr>
          <td>{$summaryItem.title}</td>
          <td>{$summaryItem.details}</td>
          <td>{$summaryItem.date}</td>
        </tr>
        {/foreach}
        </tbody>
      </table>
    </div>
</div>
{if $groupSubscriptions}

<h3>{ts}Group Subscription Log{/ts}</h3>

<div class="crm-block crm-form-block crm-grouo-subscription-list-form-block">
    <div class="list_public_group_div">
      <input type="checkbox" name="display_public_group" id="display_public_group" value="1">
      <label for="display_public_group"> List only public groups</label>
    </div>
    <div>
      <table class="selector row-highlight" id="GroupSubscriptionListTable">
        <thead class="sticky">
        <tr>
         <th scope="col">{ts}Group{/ts}</th>
         <th scope="col">{ts}Status{/ts}</th>
         <th scope="col">{ts}Date{/ts}</th>
        </tr>
        </thead>
        <tbody>
        {foreach from=$groupSubscriptions key=id item=groupSubscription}
        <tr class="{if !$groupSubscription.is_public} display_all_groups {/if} {if !$groupSubscription.is_active} in_active_groups {/if}">
          <td>{$groupSubscription.title}</td>
          <td>{$groupSubscription.status}</td>
          <td>{$groupSubscription.date}</td>
        </tr>
        {/foreach}
        </tbody>
      </table>
    </div>
</div>

{literal}
<script>
cj(document).ready( function() {
  cj('#GroupSubscriptionListTable').DataTable({
    "order": [[ 2, "desc" ]],
  });

  cj('#display_public_group').prop('checked', true);
  cj('#GroupSubscriptionListTable .display_all_groups').hide();

  cj('#display_public_group').click(function(){
    if (cj(this).prop('checked')) {
      cj('#GroupSubscriptionListTable .display_all_groups').hide();
    }
    else{
      cj('#GroupSubscriptionListTable .display_all_groups').show();
    }
  });
});
</script>
{/literal}

{else}

<div class="messages status no-popup">
  <div class="icon inform-icon"></div>
  {ts}No group subscription have been recorded for this contact.{/ts}
</div>

{/if}
{/crmScope}
