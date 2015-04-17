(function ($) {
  // IE doesn't support hiding or disabling select options, so we have to rebuild the list. :(
  Drupal.projectReleaseRebuildSelect = function () {
    var recommended = this.value,
      choices = this;

    // Remove everything
    while (this.length > 1) {
      this.remove(1);
    }

    // Now add the choices back.
    $(this).parents('table:eq(0)').find('input.form-checkbox.supported:checked').each(function () {
      $(this).parents('tr:eq(0)').find('td:first-child').each(function () {
        choices.appendChild(new Option(this.innerHTML, this.innerHTML));
        if (this.innerHTML === recommended) {
          choices.selectedIndex = choices.length - 1;
        }
      });
    });

    // If removing a supported version changes the recommended version then highlight it.
    if (this.selectedIndex === 0 && recommended !== -1) {
      $(this).parents('table:eq(0)').find('tr:last').css('background-color', '#FFFFAA');
    }
  };

  Drupal.behaviors.projectReleaseAutoAttach = {
    attach: function (context) {
      // Set handler for clicking a radio to change the recommended version.
      $('#project-release-project-edit-form select.recommended', context).change(function () {
        $(this).parents('table:eq(0)').find('tr:last').css('background-color', '#FFFFAA');
      });

      // Set handler for clicking checkbox to toggle a version supported/unsupported.
      $('#project-release-project-edit-form input.form-checkbox.supported', context).click(function () {
        $(this).parents('table:eq(0)').find('select').each(Drupal.projectReleaseRebuildSelect);
      });

      // Go ahead and remove the unavailable choices from the recommended list.
      $('select.recommended', context).each(Drupal.projectReleaseRebuildSelect);
    }
  };
})(jQuery);
