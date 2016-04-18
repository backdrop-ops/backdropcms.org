/**
 * @file addressfield-views.js
 * Provides show/hide feature for Country + Admin area views hander.
 */
(function ($) {

  Backdrop.behaviors.addressfieldOptgroupSelect = {
    attach: function(context) {
      // Save a complete set of states in memory.
      var $states = $(context).find('.addressfield-views-admin-area').once('addressfield-state');
      if ($states.length === 0) {
        return;
      }

      // Save a list of all country administrative areas.
      var areas = {};
      $states.find('optgroup').each(function() {
        areas[this.label] = $(this).children();
        $(this).detach();
      });

      // Whenver the country changes...
      $states.closest('.views-widget').find('.addressfield-views-country').change(function() {
        var val = $(this).find('option:selected').val();
        $states.empty().append(areas[val]);
        if ($states.children().length) {
          $states.closest('.form-item').show();
        }
        else {
          $states.closest('.form-item').hide();
        }
      }).triggerHandler("change");
    }
  }

}) (jQuery);
