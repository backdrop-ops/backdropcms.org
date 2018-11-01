(function ($) {

/**
 * Attaches jQuery MultiSelect.
 */
Backdrop.behaviors.akismetMultiSelect = {
  attach: function (context) {
    if ($().chosen) {
      $(context).find('select[multiple]').chosen({
        width: '90%',
        // @see search-results.tpl.php
        no_results_text: Backdrop.t('Your search yielded no results')
      });
    }

    // Adjust the recommended display for discarding spam based on moderation
    // settings.
    $(context).find('#akismet-admin-configure-form').once(function() {
      function updateRecommendedDiscard($form) {
        $form.find('label[for="edit-akismet-discard-1"] .akismet-recommended').toggle(!$form.find('input[name="akismet[moderation]"]').is(':checked'));
        $form.find('label[for="edit-akismet-discard-0"] .akismet-recommended').toggle($form.find('input[name="akismet[moderation]"]').is(':checked'));
      }

      $(this).find('input[name="akismet[moderation]"]').change(function(e) {
        updateRecommendedDiscard($(this).closest('form'));
      });

      updateRecommendedDiscard($(this));
    });
  }
};

})(jQuery);
