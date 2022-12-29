/**
 * @file
 * Enhancements for select list configuration options.
 */

(function ($) {

  "use strict";

  Backdrop.behaviors.webformSelectLoadOptions = {};
  Backdrop.behaviors.webformSelectLoadOptions.attach = function (context) {

    $('#edit-extra-options-source', context).change(function () {
      var url = Backdrop.settings.webform.selectOptionsUrl + '/' + this.value;
      $.ajax({
        url: url,
        success: Backdrop.webform.selectOptionsLoad,
        dataType: 'json'
      });
    });
  };

  Backdrop.webform = Backdrop.webform || {};

  Backdrop.webform.selectOptionsOriginal = false;
  Backdrop.webform.selectOptionsLoad = function (result) {
    if (Backdrop.optionsElement) {
      if (result.options) {
        // Save the current select options the first time a new list is chosen.
        if (Backdrop.webform.selectOptionsOriginal === false) {
          Backdrop.webform.selectOptionsOriginal = $(Backdrop.optionElements[result.elementId].manualOptionsElement).val();
        }
        $(Backdrop.optionElements[result.elementId].manualOptionsElement).val(result.options);
        Backdrop.optionElements[result.elementId].disable();
        Backdrop.optionElements[result.elementId].updateWidgetElements();
      }
      else {
        Backdrop.optionElements[result.elementId].enable();
        if (Backdrop.webform.selectOptionsOriginal) {
          $(Backdrop.optionElements[result.elementId].manualOptionsElement).val(Backdrop.webform.selectOptionsOriginal);
          Backdrop.optionElements[result.elementId].updateWidgetElements();
          Backdrop.webform.selectOptionsOriginal = false;
        }
      }
    }
    else {
      var $element = $('#' + result.elementId);
      $element.webformProp('readonly', result.options);
      if (result.options) {
        $element.val(result.options);
      }
    }
  }

  /**
   * Make a prop shim for jQuery < 1.9.
   */
  $.fn.webformProp = $.fn.webformProp || function (name, value) {
    if (value) {
      return $.fn.prop ? this.prop(name, true) : this.attr(name, true);
    }
    else {
      return $.fn.prop ? this.prop(name, false) : this.removeAttr(name);
    }
  };

})(jQuery);
