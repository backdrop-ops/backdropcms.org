/**
 * @file
 * User Account Menu toggler.
 */
(function ($) {

  // Contributor views toggle.
  $(document).ready(function() {

    // When the link is clicked...
    $('#borg-toggle').click(function() {

      if ($('.borg-greeting ul.borg-user-menu').hasClass("closed")) {
        // Show the user menu.
        $('.borg-greeting ul.borg-user-menu').show();
        $('.borg-greeting ul.borg-user-menu').addClass('open');
        $('.borg-greeting ul.borg-user-menu').removeClass('closed');
      }
      else if ($('.borg-greeting ul.borg-user-menu').hasClass("open")) {
        // Hide the user menu.
        $('.borg-greeting ul.borg-user-menu').hide();
        $('.borg-greeting ul.borg-user-menu').addClass('closed');
        $('.borg-greeting ul.borg-user-menu').removeClass('open');
      }

      // Prevent default.
      return false;

    });

  });

})(jQuery);
