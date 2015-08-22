(function ($) {
  "use strict";

  var $defaultActiveLink, $menu, $drawer, $layoutWrapper;

  // When you click on the Handbook link, expand the sub-menu.
  $(document).ready(function() {
    // Set page variables.
    $defaultActiveLink = $('.block-system-main-menu li.active').first();
    $menu = $('.block-system-main-menu ul.menu');
    $drawer = $('.l-drawer');
    $layoutWrapper = $('body');

    // Bind to the handbook link to open/close the drawer.
    $('.block-system-main-menu a[href$="/handbook"]').click(function(e) {
      if ($(window).width() >= 1024) {
        toggleDrawer(this, '.block-borg-blocks-handbook');
        return false;
      }
    });

    // Set the active class on the current drawer, if any. Classes on the
    // overall status of the drawer are added on page load in
    // borg_preprocess_layout().
    $drawer.find('li.active').first().closest('.block').addClass('active');
  });

  /**
   * Open a particular drawer and close any existing ones.
   */
  function toggleDrawer(link, drawerSelector) {
    var $link = $(link);
    var $drawerContent = $(drawerSelector);

    // If the drawer is already open and it's clicked on, close it.
    if ($layoutWrapper.hasClass('drawer-open') && $drawerContent.hasClass('active')) {
      $menu.find('li.active').removeClass('active');
      $defaultActiveLink.addClass('active');
      $layoutWrapper.removeClass('drawer-open layout-both-sidebars');
      $layoutWrapper.addClass('drawer-closed layout-one-sidebar');
      $drawerContent.removeClass('active');
    }
    // If the the drawer is open but a new section is selected, swap contents.
    else if ($layoutWrapper.hasClass('drawer-open')) {
      $menu.find('li.active').removeClass('active');
      $drawer.find('.active').removeClass('active');
      $link.closest('li').addClass('active');
      $drawerContent.addClass('active');
    }
    // If clicking on a new drawer, open the drawer and show the new content.
    else {
      $menu.find('li.active').removeClass('active');
      $drawer.find('.active').removeClass('active');
      $link.closest('li').addClass('active');
      $drawerContent.addClass('active');
      $layoutWrapper.removeClass('drawer-closed layout-one-sidebar');
      $layoutWrapper.addClass('drawer-open layout-both-sidebars');
    }
  }
})(jQuery);
