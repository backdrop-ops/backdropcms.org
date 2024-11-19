{crmScope extensionKey='uk.co.vedaconsulting.gdpr'}
  <div class="crm-communications-preferences-form-block crm-public">

    <div class="comm-pref-block channel-block">
      <!-- Page Intro Text from Settings -->
        {if $page_intro}
          <div class="section-description">
              {ts}{$page_intro}{/ts}
          </div>
        {/if}

        {if !empty($form.activity_source)}
          <fieldset id="crm-communications-preferences-groups">
            <div class="crm-section">
              <div class="label">{$form.activity_source.label}</div>
              <div class="content">{$form.activity_source.html}</div>
            </div>
          </fieldset>
        {/if}

      <!-- if any profile has configured -->
        {if !empty($custom_pre)}
          <div id="crm-communications-preferences-profile" class="crm-public-form-item crm-group custom_pre_profile-group">
              {include file="CRM/UF/Form/Block.tpl" fields=$custom_pre}
          </div>
        {/if}

      <!-- Channels fieldset section -->
        {if $channelEleNames}
          <fieldset id="crm-communications-preferences-channels">
              {if $channels_intro}
                <legend>{$channels_intro}</legend>
              {/if}
              {foreach from=$channelEleNames item=elementName}
                <div class="crm-section">
                  <div class="label">{$form.$elementName.label}</div>
                  <div class="content">{$form.$elementName.html}</div>
                  <div class="clear"></div>
                </div>
              {/foreach}
          </fieldset>
        {/if}
    </div>

    <!-- Groups from settings -->
    <div class="comm-pref-block groups-block">

        {if $groupEleNames}
          <!-- Groups Fieldset -->
          <fieldset id="crm-communications-preferences-groups" class="groups-fieldset">
              {if $groups_heading}
                <legend>{$groups_heading}</legend>
              {/if}
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
                            <span class="group-channel-matrix">{$channelUCWords}</span>
                          {/if}
                      {/foreach}
                      </span>
                      {/if}
                  </div>
                  <div class="clear"></div>
                </div>
              {/foreach}
          </fieldset>
        {/if}

      <div class="clear"></div>
      <!-- GDPR Terms and conditions url link and checkbox -->
      <fieldset id="crm-communications-preferences-datapolicy" class="data-policy-fieldset">
          {if isset($isContactDueAcceptance) && $isContactDueAcceptance}
            <div class="data-policy-intro section-sescription">
                {$tcIntro}
            </div>
            <div class="crm-section data-policy">
              <div class="label">
                <label><span class="crm-marker" title="This field is required.">*</span></label>
              </div>
              <div class="content">
                <div class="data-policy-link">
                    {$tcLink}
                </div>
                  {$form.$tcFieldName.html}
                <label for="{$tcFieldName}">{$tcFieldlabel}</label>
              </div>
              <div class="clear"></div>
            </div>
          {else}
            <div class="crm-section">
              <div class="content">
                <span>{$tcFieldlabel}</span>
                <div class="clear"></div>
              </div>
            </div>
          {/if}
    </div>
    </fieldset>

      {if isset($isCaptcha) && $isCaptcha}
          {include file='CRM/common/ReCAPTCHA.tpl'}
      {/if}

    <div class="crm-submit-buttons">
        {include file="CRM/common/formButtons.tpl" location="bottom"}
    </div>

  </div>

{literal}
  <script type="text/javascript">
    CRM.$(function($){
      var groupChk = $('.groups-fieldset input[type="checkbox"]');
      var containerPrefix = "{/literal}{$containerPrefix}{literal}";

      groupChk.each(function() {
        checkGroupChannels(this)
      });
      groupChk.on('change', function() {
        checkGroupChannels(this);
      });

      // any email fields - trim any space around or embedded in it
      $("input[id^='email-'][type='text']").on('input',function(e){
        var trtxt = $(this).val().replace(/\s+/g, '');
        $(this).val(trtxt);
      });

      function checkGroupChannels(controller) {
        var groupId 	= $(controller).attr('id')
        var groupDiv	= $(controller).parent('.group-channel-div');
        var isChecked = $(controller).is(':checked');

        var mismatchedChannels = [];
        if (isChecked) {
          $(groupDiv).find('.group-channel-matrix').each(function(){
            var groupChannel = $.trim($(this).text().toLowerCase());
            var currentChannelValue = $('#' + containerPrefix + groupChannel).val();
            if (currentChannelValue != 'YES') {
              mismatchedChannels.push(groupChannel);
            }
          });

          if (mismatchedChannels.length !== 0) {
            var mismatchedChannelTxt = mismatchedChannels.join(', ');
            CRM.confirm({
              title: {/literal}'{ts escape="js"}Group Channels{/ts}'{literal},
              message: ts('We may communicate with you by %1 since this is used by a group you have selected.',  {1: '<em>' + mismatchedChannelTxt + '</em>'})
            })
              .on('crmConfirm:yes', function() {
                $(mismatchedChannels).each(function(index, value){
                  $('#'+containerPrefix + value).val('YES');
                });
              });
          }
        }
      }
    });
  </script>
{/literal}
{/crmScope}
