CRM.$(function ($) {

  function showHideElasticSettings() {
    console.log('hi');
    if ($('select#external_smtp_service').val() == 'Elastic') {
      $('tr.eesettings').show();
    }
    else {
      $('tr.eesettings').hide();
    }
  }

  showHideElasticSettings();
  $('select#external_smtp_service').change(showHideElasticSettings);
});
