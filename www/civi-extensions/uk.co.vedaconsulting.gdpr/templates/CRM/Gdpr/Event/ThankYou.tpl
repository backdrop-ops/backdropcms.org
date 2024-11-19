{* Communication Preferences prepended to Event Registration and Contribution Thank You pages *}
{if $comm_pref_in_thankyou eq 'link'}
  <div id="comm_pref_url" class="comm_pref_url_div">
    <div class="header-dark"> Communication Preferences </div>
    {if $link_intro}
      <div id="comm_pref_intro">
        <span>{$link_intro}</span>
      </div>
    {/if}
    <div id="comm_pref_link">
      <span><a href="{$comm_pref_url}">{$link_label}</a></span>
    </div>
  </div>

{literal}
  <script type="text/javascript">
    (function($) {
      var entity = "{/literal}{$entity}{literal}";
      if (entity === 'Event') {
        $('#comm_pref_url').prependTo($('#crm-main-content-wrapper'));
      }
      else{
        $('#comm_pref_url').prependTo($('#crm-main-content-wrapper'));
      }
    }(CRM.$))
  </script>
{/literal}
{/if}
{if $comm_pref_in_thankyou eq 'embed' && !$noperm}
  <div id="comms_pref_form">
    {if $commPrefIntro }
      {$commPrefIntro}
    {/if}

    <!-- channels fieldset -->
    <fieldset id="comm_pref_fields">
      <div class="section-description">{$channels_intro}</div>
      {foreach from=$channelEleNames item=elementName}
        <div class="crm-section">
          <div class="label">{$form.$elementName.label}</div>
          <div class="content">{$form.$elementName.html}</div>
          <div class="clear"></div>
        </div>
      {/foreach}
    </fieldset>

    <!-- Groups Fieldset -->
    <fieldset id="comm_pref_groups" class="groups-fieldset">
      {if $groups_intro}
        <div class="section-description">
          {ts}{$groups_intro}{/ts}
        </div>
      {/if}
      {foreach from=$groupEleNames item=elementName}
        <div class="crm-section">
          <div class="content group-channel-div">
            {$form.$elementName.html}
            {$form.$elementName.label}
            {if $commPrefGroupsetting.$elementName.group_description}
              <br>
              <span class="group-description">
              {$commPrefGroupsetting.$elementName.group_description}
              <br>
              {foreach from=$groupChannel key=channelNoPrefix item=channelUCWords}
                {if $commPrefGroupsetting.$elementName.$channelNoPrefix}
                  <span class="group-channel-matrix" style="display:none;">{$channelUCWords}</span>
                {/if}
              {/foreach}
            </span>
            {/if}
          </div>
          <div class="clear"></div>
        </div>
      {/foreach}
    </fieldset>
    <div class="submit-buttons">
      <input type="button" name="comm_pref_submit" id="comm_pref_submit" value="Submit" class="crm-button" />
    </div>
  </div>

{literal}
  <script type="text/javascript">
    (function($) {
      var entity = "{/literal}{$entity}{literal}";
      var contactId = "{/literal}{$contactId}{literal}";
      var checksum = "{/literal}{$contact_cs}{literal}";
      var channelEleNames = {/literal}{$channelEleNamesJSON}{literal};
      var groupEleNames = {/literal}{$groupEleNamesJSON}{literal};
      if (entity === 'Event') {
        $('#comms_pref_form').prependTo($('#crm-main-content-wrapper'));
      }
      else{
        $('#comms_pref_form').prependTo($('#crm-main-content-wrapper'));
      }

      //On comms preference submission
      $("#comm_pref_submit").click(function(){
        var ajaxURL = {/literal}'{crmURL p="civicrm/ajax/rest" h=0 q="className=CRM_Gdpr_Page_AJAX&fnName=commPreferenceSubmission&json=1"}'{literal};

        //gather all information we need to update comms preference
        var updatePreferenceData = {};
        $(channelEleNames).each(function(index, value){
          updatePreferenceData[value] = $('#'+value).val();
        });

        //gather all groups Informations
        $(groupEleNames).each(function(index, value){
          if ($('#'+value).prop('checked')) {
            updatePreferenceData[value] = 1;
          }
          else{
            updatePreferenceData[value] = 0;
          }
        });

        $.ajax({
          type: "POST",
          url: ajaxURL,
          data: {
            contactId : contactId,
            contact_cs : checksum,
            preference :  updatePreferenceData
          },
          success: function (responseText) {
            $("#comms_pref_form").html("<div class='header-dark'>"+responseText+"</div><br>");
          } //end of success
        }); //end of ajax
      });
    }(CRM.$))
  </script>
{/literal}
{/if}
