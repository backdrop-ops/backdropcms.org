
/**
 * @file
 * Webform node form interface enhancements.
 */

(function ($) {

  "use strict";

  Backdrop.behaviors.webformAdmin = {};
  Backdrop.behaviors.webformAdmin.attach = function (context) {
    // On click or change, make a parent radio button selected.
    Backdrop.webform.setActive(context);
    Backdrop.webform.updateTemplate(context);
    // Update the template select list upon changing a template.
    // Select all link for file extensions.
    Backdrop.webform.selectCheckboxesLink(context);
    // Enhance the normal tableselect.js file to support indentations.
    Backdrop.webform.tableSelectIndentation(context);
    // Automatically download exports if available.
    Backdrop.webform.downloadExport(context);
    // Enhancements for the conditionals administrative page.
    Backdrop.webform.conditionalAdmin(context);
    // Trigger radio/checkbox change when label click automatically selected by
    // browser.
    Backdrop.webform.radioLabelAutoClick(context);
  };

  Backdrop.webform = Backdrop.webform || {};

  Backdrop.webform.setActive = function (context) {
    $('.webform-inline-radio', context).click(function (e) {
      $(this).closest('.form-type-radio').find('input[type=radio]').webformProp('checked', true);
    });
    $('.webform-set-active', context).change(function (e) {
      if ($(this).val()) {
        $(this).closest('.form-type-radio').find('input[type=radio]').webformProp('checked', true);
      }
      e.preventDefault();
    });

    // Firefox improperly selects the parent radio button when clicking inside
    // a label that contains an input field. The only way of preventing this
    // currently is to remove the "for" attribute on the label.
    // See https://bugzilla.mozilla.org/show_bug.cgi?id=213519.
    if (navigator.userAgent.match(/Firefox/)) {
      $('.webform-inline-radio', context).removeAttr('for');
    }
  };

  // Update e-mail templates between default and custom.
  Backdrop.webform.updateTemplate = function (context) {
    var defaultTemplate = $('#edit-templates-default').val();
    var $templateSelect = $('#webform-template-fieldset select#edit-template-option', context);
    var $templateTextarea = $('#webform-template-fieldset textarea:visible', context);

    var updateTemplateSelect = function () {
      if ($(this).val() == defaultTemplate) {
        $templateSelect.val('default');
      }
      else {
        $templateSelect.val('custom');
      }
    };

    var updateTemplateText = function () {
      if ($(this).val() == 'default' && $templateTextarea.val() != defaultTemplate) {
        if (confirm(Backdrop.settings.webform.revertConfirm)) {
          $templateTextarea.val(defaultTemplate);
        }
        else {
          $(this).val('custom');
        }
      }
    };

    $templateTextarea.keyup(updateTemplateSelect);
    $templateSelect.change(updateTemplateText);
  };

  Backdrop.webform.selectCheckboxesLink = function (context) {
    function selectCheckboxes() {
      var group = this.className.replace(/.*?webform-select-link-([^ ]*).*/, '$1');
      var $checkboxes = $('.webform-select-group-' + group + ' input[type=checkbox]');
      var reverseCheck = !$checkboxes[0].checked;
      $checkboxes.each(function () {
        this.checked = reverseCheck;
      });
      $checkboxes.trigger('change');
      return false;
    }
    $('a.webform-select-link', context).click(selectCheckboxes);
  };

  Backdrop.webform.tableSelectIndentation = function (context) {
    var $tables = $('th.select-all', context).parents('table');
    $tables.find('input.form-checkbox').change(function () {
      var $rows = $(this).parents('table:first').find('tr');
      var row = $(this).parents('tr:first').get(0);
      var rowNumber = $rows.index(row);
      var rowTotal = $rows.size();
      var indentLevel = $(row).find('div.indentation').size();
      for (var n = rowNumber + 1; n < rowTotal; n++) {
        if ($rows.eq(n).find('div.indentation').size() <= indentLevel) {
          break;
        }
        $rows.eq(n).find('input.form-checkbox').webformProp('checked', this.checked);
      }
    });
  };

  /**
   * Attach behaviors for Webform results download page.
   */
  Backdrop.webform.downloadExport = function (context) {
    if (context === document && Backdrop.settings && Backdrop.settings.webformExport && document.cookie.match(/webform_export_info=1/)) {
      window.location = Backdrop.settings.webformExport;
      delete Backdrop.settings.webformExport;
    }
  };

  /**
   * Attach behaviors for Webform conditional administration.
   */
  Backdrop.webform.conditionalAdmin = function (context) {
    var $context = $(context);
    // Bind to the entire form and allow events to bubble-up from elements. This
    // saves a lot of processing when new conditions are added/removed.
    $context.find('#webform-conditionals-ajax:not(.webform-conditional-processed)')
      .addClass('webform-conditional-processed')
      .bind('change', function (e) {

        var $target = $(e.target);
        if ($target.is('.webform-conditional-source select')) {
          Backdrop.webform.conditionalSourceChange.apply(e.target);
        }

        if ($target.is('.webform-conditional-operator select')) {
          Backdrop.webform.conditionalOperatorChange.apply(e.target);
        }

        if ($target.is('.webform-conditional-andor select')) {
          Backdrop.webform.conditionalAndOrChange.apply(e.target);
        }

        if ($target.is('.webform-conditional-action select')) {
          Backdrop.webform.conditionalActionChange.apply(e.target);
        }
      });

    // Add event handlers to delete the entire row if the last rule or action is removed.
    $context.find('.webform-conditional-rule-remove:not(.webform-conditional-processed)').bind('click', function () {
      this.webformRemoveClass = '.webform-conditional-rule-remove';
      window.setTimeout($.proxy(Backdrop.webform.conditionalRemove, this), 100);
    }).addClass('webform-conditional-processed');
    $context.find('.webform-conditional-action-remove:not(.webform-conditional-processed)').bind('click', function () {
      this.webformRemoveClass = '.webform-conditional-action-remove';
      window.setTimeout($.proxy(Backdrop.webform.conditionalRemove, this), 100);
    }).addClass('webform-conditional-processed');

    // Trigger default handlers on the source element, this in turn will trigger
    // the operator handlers.
    $context.find('.webform-conditional-source select').trigger('change');

    // Trigger defaults handlers on the action element.
    $context.find('.webform-conditional-action select').trigger('change');

    // When adding a new table row, make it draggable and hide the weight column.
    if ($context.is('tr.ajax-new-content') && $context.find('.webform-conditional').length === 1) {
      Backdrop.tableDrag['webform-conditionals-table'].makeDraggable($context[0]);
      $context.find('.webform-conditional-weight').closest('td').addClass('tabledrag-hide');
      if ($.cookie('Backdrop.tableDrag.showWeight') !== '1') {
        Backdrop.tableDrag['webform-conditionals-table'].hideColumns();
      }
      $context.removeClass('ajax-new-content');
    }
  };

  /**
   * Event callback for the remove button next to an individual rule.
   */
  Backdrop.webform.conditionalRemove = function () {
    // See if there are any remaining rules in this element.
    var rowCount = $(this).parents('.webform-conditional:first').find(this.webformRemoveClass).length;
    if (rowCount <= 1) {
      var $tableRow = $(this).parents('tr:first');
      var $table = $('#webform-conditionals-table');
      if ($tableRow.length && $table.length) {
        $tableRow.remove();
        Backdrop.webform.restripeTable($table[0]);
      }
    }
  };

  /**
   * Event callback to update the list of operators after a source change.
   */
  Backdrop.webform.conditionalSourceChange = function () {
    var source = $(this).val();
    var dataType = Backdrop.settings.webform.conditionalValues.sources[source]['data_type'];
    var $operator = $(this).parents('.webform-conditional-rule:first').find('.webform-conditional-operator select');

    // Store a the original list of all operators for all data types in the select
    // list DOM element.
    if (!$operator[0]['webformConditionalOriginal']) {
      $operator[0]['webformConditionalOriginal'] = $operator[0].innerHTML;
    }

    // Reference the original list to create a new list matching the data type.
    var $originalList = $($operator[0]['webformConditionalOriginal']);
    var $newList = $originalList.filter('optgroup[label=' + dataType + ']');
    var newHTML = $newList[0].innerHTML;

    // Update the options and fire the change event handler on the list to update
    // the value field, only if the options have changed. This avoids resetting
    // existing selections.
    if (newHTML != $operator.html()) {
      $operator.html(newHTML);
    }
    // Trigger the change in case the source component changed from one select
    // component to another.
    $operator.trigger('change');
  };

  /**
   * Event callback to update the value field after an operator change.
   */
  Backdrop.webform.conditionalOperatorChange = function () {
    var source = $(this).parents('.webform-conditional-rule:first').find('.webform-conditional-source select').val();
    var dataType = Backdrop.settings.webform.conditionalValues.sources[source]['data_type'];
    var operator = $(this).val();
    var $value = $(this).parents('.webform-conditional-rule:first').find('.webform-conditional-value');
    var name = $value.find('input, select, textarea').attr('name');
    var originalValue = false;

    // Given the dataType and operator, we can determine the form key.
    var formKey = Backdrop.settings.webform.conditionalValues.operators[dataType][operator]['form'];
    var formSource = typeof Backdrop.settings.webform.conditionalValues.forms[formKey] == 'undefined' ? false : source;

    // On initial request, save the default field as printed on the original page.
    if (!$value[0]['webformConditionalOriginal']) {
      $value[0]['webformConditionalOriginal'] = $value[0].innerHTML;
      originalValue = $value.find('input:first').val();
    }
    // On changes to an existing operator, check if the form key is different
    // (and any per-source form, such as a select option list) before replacing
    // the form with an identical version.
    else if ($value[0]['webformConditionalFormKey'] == formKey && $value[0]['webformConditionalFormSource'] == formSource) {
      return;
    }

    // Store the current form key for checking the next time the operator changes.
    $value[0]['webformConditionalFormKey'] = formKey;
    $value[0]['webformConditionalFormSource'] = formSource;

    // If using the default (a textfield), restore the original field.
    if (formKey === 'default') {
      $value[0].innerHTML = $value[0]['webformConditionalOriginal'];
    }
    // If the operator does not need a source value (i.e. is empty), hide it.
    else if (formKey === false) {
      $value[0].innerHTML = '<input type="text" value="" style="display: none;" >';
    }
    // If there is a per-source form for this operator (e.g. option lists), use
    // the specialized value form.
    else if (typeof Backdrop.settings.webform.conditionalValues.forms[formKey] == 'object') {
      $value[0].innerHTML = Backdrop.settings.webform.conditionalValues.forms[formKey][source];
    }
    // Otherwise all the sources use a generic field (e.g. a text field).
    else {
      $value[0].innerHTML = Backdrop.settings.webform.conditionalValues.forms[formKey];
    }

    // Set the name attribute to match the original placeholder field.
    var $firstElement = $value.find('input, select, textarea').filter(':first');
    $firstElement.attr('name', name);

    if (originalValue) {
      $firstElement.val(originalValue);
    }
  };

  /**
   * Event callback to make sure all group and/or operators match.
   */
  Backdrop.webform.conditionalAndOrChange = function () {
    var rid = this.getAttribute('data-rid');
    var text = $(this).find('option:selected').text();
    $(this).parents('.webform-conditional:first').find('.webform-conditional-andor div[data-rid="' + rid + '"]').text(text);
  };

  /**
   * Event callback to show argument only for appropriate actions.
   */
  Backdrop.webform.conditionalActionChange = function () {
    var action = $(this).val();
    var $argument = $(this).parents('.webform-conditional-condition:first').find('.webform-conditional-argument input');
    var isShown = $argument.is(':visible');
    switch (action) {
      case 'show':
      case 'require':
        if (isShown) {
          $argument.hide();
        }
        break;

      case 'set':
        if (!isShown) {
          $argument.show();
        }
        break;
    }
  };

  /**
   * Triggers a change event when a label receives a click.
   *
   * When the browser automatically selects a radio button when it's label is
   * clicked, the FAPI states jQuery code doesn't receive an event. This function
   * ensures that automatically-selected radio buttons keep in sync with the
   * FAPI states.
   */
  Backdrop.webform.radioLabelAutoClick = function (context) {
    $('label').once('webform-label').click(function () {
      $(this).prev('input:radio').change();
    });
  };

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
