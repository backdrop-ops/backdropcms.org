/**
 * @file
 * Setting default value of ajax select to none on window load.
 * Setting checkboxes to automatically check, uncheck or disable
 * on instance settings table form.
 */

(function ($) {
	$(window).load(function() {
	  $('.add_one_social_service').val('_none');
		$('#socialfield-instance-settings-services-table .socialfield-table-displayed-service-checkbox').click(function() {
			// Getting elements status.
			var displayedServiceStatus = $(this).attr('checked');
			var usedService = $(this).closest('tr').find('.socialfield-table-used-service-checkbox');
			var usedServiceStatus = usedService.attr('checked');

			if (displayedServiceStatus) {
				// Saving the original status of usedService.
				usedService.data('original-status', usedServiceStatus);
				usedService.attr('checked', true);
				usedService.attr('disabled', true);
			} else {
				usedService.attr('checked', usedService.data('original-status'));
				usedService.removeAttr('disabled');
			}

		});
	});
})(jQuery);
