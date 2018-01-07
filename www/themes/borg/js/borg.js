/**
 * @file
 * User Account Menu toggler.
 */
(function ($) {

  // Contributor views toggle.
  $(document).ready(function() {

    // When the link is clicked...
    $('#greeting').click(function() {

      if ($('.borg-greeting ul.menu-tree').hasClass("closed")) {
        // Show the user menu.
        $('.borg-greeting ul.menu-tree').show();
        $('.borg-greeting ul.menu-tree').addClass('open');
        $('.borg-greeting ul.menu-tree').removeClass('closed');
      }
      else if ($('.borg-greeting ul.menu-tree').hasClass("open")) {
        // Hide the user menu.
        $('.borg-greeting ul.menu-tree').hide();
        $('.borg-greeting ul.menu-tree').addClass('closed');
        $('.borg-greeting ul.menu-tree').removeClass('open');
      }

      // Prevent default.
      return false;

    });

  });

})(jQuery);
