<div class="crm-block crm-form-block crm-airmail-form-block">

  <table class="form-layout-compressed">

    <tr>
      <td class="label">{$form.external_smtp_service.label}</td>
      <td>{$form.external_smtp_service.html}<br />
      </td>
    </tr>

    <tr>
      <td class="label">{$form.secretcode.label}</td>
      <td>{$form.secretcode.html}<br />
        <span class="description">{ts}You may provide a secret code here and in the notification URL in order to discourage spoof event notifications.{/ts}</span>
      </td>
    </tr>
    
    <tr class='eesettings'>
      <td class="label"></td>
      <td>
        <div>{$form.ee_wrapunsubscribe.html} {$form.ee_wrapunsubscribe.label}</div>
        <div class='description'>Checking this box without first negotiating this with Elastic Email will mean you are no longer to email anyone who unsubscribes to any group. See documentation!</div>
        <div class='label'>
          {$form.ee_unsubscribe.label}
        </div>
        <div>
          {$form.ee_unsubscribe.html}
          <div class="description">{ts}If you do not include an Elastic Email {literal}{unsubscribe}{/literal} link in your emails, one will be added at the bottom of the email, following this explainer text. See documentation for fuller explanation. Note that the text you enter is transalatable.{/ts}</div>
        </div>
      </td>
    </tr>
  </table>
  <div class="crm-submit-buttons">{include file="CRM/common/formButtons.tpl"}</div>
  <div class="spacer"></div>

  <div class="help">
    <h3>{ts}Airmail Event Notification Configuration{/ts}</h3>
    {*<p>{ts}We should probably put a link here to the event notification setup screen on Airmail.{/ts}</p>*}
    <p>Based on the secret code provided above, your <em>HTTP Post URL</em> is...</p>
    <pre>{$url}</pre>
    <p>{ts}see README.md for more details on how to configure your external SMTP service{/ts}</p>
  </div>

</div>
