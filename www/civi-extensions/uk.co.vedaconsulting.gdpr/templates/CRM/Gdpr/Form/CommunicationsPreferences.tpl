{crmScope extensionKey='uk.co.vedaconsulting.gdpr'}
{* Template for the Communications Preferences Settings form. *}

<div>
   <div class="help">
   {if $communications_preferences_page_url}
   {ts}Configure the display of the <a title="Your personalised GDPR page. Do not distribute this URL as it allows access to your data. Use tokens or the Contact Action link instead." href="{$communications_preferences_page_url}" target="blank">Communications Preferences page</a>.{/ts}
   {/if}
   </div>
  <div class="crm-block crm-form-block crm-gdpr-comms-prefs-form-block">
  {foreach from=$page_elements item=elementName}
    <div class="crm-section">
      <div class="label">{$form.$elementName.label}</div>
      <div class="content">{$form.$elementName.html}
      {if array_key_exists($elementName, $descriptions)}
        <div class="description">{$descriptions.$elementName}</div>
      {/if}
      </div>
      <div class="clear"></div>
    </div>
  {/foreach}
  </div>{* end page block *}
  <h3> {ts}Channels{/ts} </h3>
   <div class="help">
   {ts}Configure which channels to show on the page.{/ts}
   </div>
  {* Channels block *}
  <div class="crm-block crm-form-block crm-gdpr-comms-prefs-form-block">
    <div class="crm-section">
      <div class="label">{$form.enable_channels.label}</div>
      <div class="content">{$form.enable_channels.html}
      </div>
      <div class="clear"></div>
    </div>
  <fieldset class="channels-wrapper toggle-target">
  {foreach from=$channels_elements item=elementName}
    <div class="crm-section">
      <div class="label">{$form.$elementName.label}</div>
      <div class="content">{$form.$elementName.html}</div>
      <div class="clear"></div>
    </div>
  {/foreach}
   </fieldset>
  </div>{* end Channels block *}
  <h3> {ts}Subscriptions to Groups{/ts} </h3>
   <div class="help">
   {ts}Configure which public groups the user can join on the page. You can optionally alter the group title and description. (For example to add more details on the content, message frequency etc.).{/ts}
   </div>
  {* Groups block *}
  <div class="crm-block crm-form-block crm-gdpr-comms-prefs-form-block">
    <div class="crm-section">
      <div class="label">{$form.enable_groups.label}</div>
      <div class="content">{$form.enable_groups.html}</div>
      <div class="clear"></div>
    </div>
   <fieldset class="groups-wrapper toggle-target">
  {foreach from=$groups_elements item=elementName}
    <div class="crm-section">
      <div class="label">{$form.$elementName.label}</div>
      <div class="content">{$form.$elementName.html}</div>
      <div class="clear"></div>
    </div>
  {/foreach}
  {if $group_containers}
   <table>
   <tr>
   <th>{ts}Group{/ts}</th><th>{ts}Add{/ts}</th><th>{ts}Title{/ts}</th><th>{ts}Weight{/ts}</th><th>{ts}Description{/ts}</th><th>{ts}Channel{/ts}</th>
   </tr>
   {foreach from=$group_containers item=containerName}
     <tr>
     <td>
     <strong>
     {$form.$containerName.label}
     </strong>
     </td>
      <td>
       {$form.$containerName.group_enable.html}
      </td>
      <td>
       {$form.$containerName.group_title.html}
      </td>
      <td>
       {$form.$containerName.group_weight.html}
      </td>
      <td>
       {$form.$containerName.group_description.html}
      </td>
      <td class="group-channels">
        {$form.$containerName.email.html}
        {$form.$containerName.phone.html}
        {$form.$containerName.post.html}
        {$form.$containerName.sms.html}
      </td>

     </tr>
   {/foreach}

   </table>
   {/if}
   </fieldset>
  </div>{* end Groups block *}
  <h3> {ts}Completion{/ts} </h3>
  <div class="crm-block crm-form-block crm-gdpr-comms-prefs-form-block">
    <div class="crm-section">
      <div class="label">{$form.completion_redirect.label}</div>
      <div class="content">{$form.completion_redirect.html}
        <div class="description"> </div>
      </div>
    </div>
    <div class="clear"></div>
    <div class="crm-section completion-message">
      <div class="label">{$form.completion_message.label}</div>
      <div class="content">{$form.completion_message.html}
        <div class="description">{$descriptions.completion_message}</div>
      </div>
    </div>
    <div class="clear"></div>
    <div class="crm-section completion-url">
      <div class="label">{$form.completion_url.label}</div>
      <div class="content">{$form.completion_url.html}
        <div class="description">{$descriptions.completion_url}</div>
      </div>
    </div>
    <div class="clear"></div>
  </div> {* end Completion block *}
  <h3> {ts}Event & Contribution thank you page{/ts} </h3>
  <div class="crm-block crm-form-block crm-gdpr-comms-prefs-form-block">
    <div class="help">{ts}<p>Allows supporters set their preferences after registering for an event or making a contribution.</p><p>Note, to embed the Communcation Preferences form in thank-you pages, you need to give anonymous users the permission: <em>CiviCRM: access AJAX API</em>.</p>{/ts}</div>
    <div class="crm-section">
      <div class="label">{$form.comm_pref_in_thankyou.label}</div>
      <div class="content">{$form.comm_pref_in_thankyou.html}
        <div class="description"> </div>
      </div>
    </div>
    <div class="clear"></div>
    <div class="crm-section thank-you-embed-wrapper">
      <div class="label">{$form.comm_pref_thankyou_embed_intro.label}</div>
      <div class="content">{$form.comm_pref_thankyou_embed_intro.html}</div>
        <div class="clear"> </div>
      <div class="label">{$form.comm_pref_thankyou_embed_complete_msg.label}</div>
      <div class="content">{$form.comm_pref_thankyou_embed_complete_msg.html}</div>
        <div class="description"> </div>
    </div>
    <div class="crm-section thank-you-link-wrapper">
      <div class="label">{$form.comm_pref_link_label.label}</div>
      <div class="content">{$form.comm_pref_link_label.html}</div>
      <div class="clear"></div>
      <div class="label">{$form.comm_pref_link_intro.label}</div>
      <div class="content">{$form.comm_pref_link_intro.html}</div>
    </div>
    <div class="clear"></div>
  </div> {* end Completion block *}
    {* Confirmation Email Block *}
  <fieldset id="mail" class="crm-collapsible {if isset($defaultsEmpty) && $defaultsEmpty}collapsed{/if}">
    <legend class="collapsible-title">{ts}Confirmation Email{/ts}</legend>
    <div>
      <table class="form-layout-compressed">
        <tr class="crm-event-manage-registration-form-block-is_email_confirm">
          <td scope="row" class="label" width="20%">{$form.is_email_confirm.label}</td>
          <td>{$form.is_email_confirm.html}<br/>
            <span
                    class="description">{ts}Do you want a confirmation email sent automatically to the user?{/ts}</span>
          </td>
        </tr>
      </table>
      <div id="confirmEmail">
        <table class="form-layout-compressed">
          <tr class="crm-event-manage-registration-form-block-confirm_email_text">
            <td scope="row" class="label"
                width="20%">{$form.confirm_email_text.label} {if $action == 2}{include file='CRM/Core/I18n/Dialog.tpl' table='civicrm_event' field='confirm_email_text' id=$eventID}{/if}</td>
            <td>{$form.confirm_email_text.html}<br/>
              <span
                      class="description">{ts}Additional message or instructions to include in confirmation email.{/ts}</span>
            </td>
          </tr>
          <tr class="crm-event-manage-registration-form-block-confirm_from_name">
            <td scope="row" class="label" width="20%">{$form.confirm_from_name.label} <span
                      class="crm-marker">*</span> {if $action == 2}{include file='CRM/Core/I18n/Dialog.tpl' table='civicrm_event' field='confirm_from_name' id=$eventID}{/if}
            </td>
            <td>{$form.confirm_from_name.html}<br/>
              <span class="description">{ts}FROM name for email.{/ts}</span>
            </td>
          </tr>
          <tr class="crm-event-manage-registration-form-block-confirm_from_email">
            <td scope="row" class="label" width="20%">{$form.confirm_from_email.label} <span class="crm-marker">*</span></td>
            <td>{$form.confirm_from_email.html}<br/>
              <span
                      class="description">{ts}FROM email address (this must be a valid email account with your SMTP email service provider).{/ts}</span>
            </td>
          </tr>
          <tr class="crm-event-manage-registration-form-block-cc_confirm">
            <td scope="row" class="label" width="20%">{$form.cc_confirm.label}</td>
            <td>{$form.cc_confirm.html}<br/>
              <span
                      class="description">{ts}You may specify one or more email addresses to receive a carbon copy (cc). Multiple email addresses should be separated by a comma (e.g. jane@example.org, paula@example.org).{/ts}</span>
            </td>
          </tr>
          <tr class="crm-event-manage-registration-form-block-bcc_confirm">
            <td scope="row" class="label" width="20%">{$form.bcc_confirm.label}</td>
            <td>{$form.bcc_confirm.html}<br/>
              <span
                      class="description">{ts}You may specify one or more email addresses to receive a blind carbon copy (bcc) of the confirmation email. Multiple email addresses should be separated by a comma (e.g. jane@example.org, paula@example.org).{/ts}</span>
            </td>
          </tr>
        </table>
      </div>
    </div>
  </fieldset>
 </div>{* end form *}

{* FOOTER *}
<div class="crm-submit-buttons">
{include file="CRM/common/formButtons.tpl" location="bottom"}
</div>
{literal}
<script>
(function($) {
  // General toggle
  $('input.toggle-control').on('change', function(){
    var toggleTarget = $(this).data('toggle');
    if (toggleTarget) {
      $(toggleTarget).toggle($(this).is(':checked'));
    }
  }).trigger('change');
  $('input.toggle-select').on('change', function() {
    var map = $(this).data('toggle-mapping'),
    currVal = $(this).val();
    if (!$(this).prop('checked')) {
      return;
    }
    for (val in map) {
      if (!map[val]) {
        continue;
      }
      $(map[val]).toggle(val == currVal);
    }
  }).trigger('change');
  // Toggle completion setting elements.
  var completionRadioSelector = 'input[name="completion_redirect"]',
    completionRadio = $(completionRadioSelector),
    completionUrl = $('.completion-url');
    completionMessage = $('.completion-message'),
    isOn = $(completionRadioSelector + ":checked").val() == 1;
  completionUrl.toggle(isOn);
  completionMessage.toggle(!isOn);
  completionRadio.on('change', function(){
    var isOn = (true == $(this).val());
    completionUrl.toggle(isOn);
    completionMessage.toggle(!isOn);
  });
  controlGroupChannels();
  function controlGroupChannels() {
    var channels = ['email', 'phone', 'post', 'sms'];
    var channelChk = $('input.enable-channel');
    var groupChannels = $('.group-channels input[type="checkbox"]');
    // Disable group channels if channel is disabled.
    function checkGroupChannels(controller) {
        var channel = $(controller).attr('id').replace('channels_enable_', '');
        if (channels.indexOf(channel) === false) {
          return;
        }
        var isChecked = $(controller).is(':checked');
        // get group channels
        groupChannels.filter('input[id$="' + channel + '"]').each(function(){
            if (!isChecked) {
              $(this).attr('checked', false);
            }
            $(this).attr('disabled', !isChecked);
          });
    }
    channelChk.each(function() {
      checkGroupChannels(this)
    });
    channelChk.on('change', function() {
      checkGroupChannels(this);
    });
  }
}(cj));
</script>
{/literal}
{/crmScope}

{include file="CRM/common/showHideByFieldValue.tpl"
trigger_field_id    ="is_email_confirm"
trigger_value       =""
target_element_id   ="confirmEmail"
target_element_type ="block"
field_type          ="radio"
invert              = 0
}
