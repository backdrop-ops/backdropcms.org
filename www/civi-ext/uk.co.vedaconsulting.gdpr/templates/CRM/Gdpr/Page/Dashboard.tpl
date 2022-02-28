{crmScope extensionKey='uk.co.vedaconsulting.gdpr'}
<h3>{ts 1=$settings.activity_period}Contacts who have not had any activity for %1 days{/ts}</h3>
<div class="crm-block crm-form-block crm-gdpr-dashboard-activities-list-form-block">
    <div>
      <table class="selector row-highlight" id="ContactSummaryListTable">
        <thead class="sticky">
        <tr>
          <th scope="col" width="60%">{ts}Activity Types{/ts}</th>
          <th scope="col">{ts}No. of Contacts{/ts}</th>
        </tr>
        </thead>
        <tbody>
        <tr>
          <td>{$gdprActTypes}
          <br />
          <span class="description"><i>{ts}(Excluding contacts who clicked through links in emails){/ts}</i></span>
          </td>
          <td>
            {if $actContactCsDetails.id}
              {capture assign=actContactCustomSearchUrl}{crmURL p="civicrm/contact/search/custom" q="reset=1&force=1&csid=`$actContactCsDetails.id`"}{/capture}
              <a href="{$actContactCustomSearchUrl}">{$count}</a>
            {else}
              <a href='{crmURL p="civicrm/gdpr/activitycontact" q="reset=1"}'>{$count}</a>
            {/if}
          </td>
        </tr>
        <tr>
          <td>{ts}Click-throughs{/ts}
          <br />
          <span class="description"><i>{ts}(Contacts who have not had any activity, but clicked through links in emails){/ts}</i></span>
          </td>
          <td>{$clickThroughCount}</td>
        </tr>
        </tbody>
      </table>
    </div>
</div>

{if $gsCsDetails}
<h3>{ts}Search{/ts}</h3>
<div class="crm-block crm-form-block crm-gdpr-dashboard-search-form-block">
{capture assign=customSearchUrl}{crmURL p="civicrm/contact/search/custom" q="reset=1&csid=`$gsCsDetails.id`"}{/capture}
    <a href="{$customSearchUrl}">{$gsCsDetails.label}</a>
</div>
{/if}

{if call_user_func(array('CRM_Core_Permission','check'), 'administer GDPR')}
<h3>{ts}GDPR Settings{/ts}</h3>
<div class="crm-block crm-form-block crm-gdpr-dashboard-settings-form-block">
    <div>
        <div id="help">
          <ul>
           {capture assign=GDPRSettingsUrl}{crmURL p="civicrm/gdpr/settings" q="reset=1"}{/capture}
           <li>{ts 1=$GDPRSettingsUrl}Click <a href="%1">here</a> to update GDPR settings.{/ts}</li>
           {capture assign=CommPrefSettingsUrl}{crmURL p="civicrm/gdpr/comms-prefs/settings" q="reset=1"}{/capture}
           <li>{ts 1=$CommPrefSettingsUrl}Click <a href="%1">here</a> to update Communications Preferences settings.{/ts}</li>
          </ul>
        </div>
    </div>
</div>
{/if}
{/crmScope}
