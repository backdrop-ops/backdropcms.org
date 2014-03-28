(function ($) {
  var $defaultActiveLink;

  // When you click on the Handbook link, expand the sub-menu.
  $(document).ready(function() {
    $defaultActiveLink = $('#main-menu li.active').first();
    $('.menu-320 a').click(function(e) {
      if ($(window).width() >= 1024) {
        toggleDrawer(this, '#handbook-menu');
        return false;
      }
    });
  });

  /**
   * Open a particular drawer and close any existing ones.
   */
  function toggleDrawer(link, drawerSelector) {
    $link = $(link);
    $menu = $('#main-menu');
    $drawer = $('#drawer');
    $drawerContent = $(drawerSelector);
    $pageWrapper = $('#main-wrapper');

    // If the drawer is already open and it's clicked on, close it.
    if ($drawerContent.hasClass('active')) {
      $menu.find('li.active').removeClass('active');
      $defaultActiveLink.addClass('active');
      $pageWrapper.removeClass('drawer-open');
      $drawerContent.removeClass('active');
      $drawer.removeClass('open');
    }
    // If the the drawer is open but a new section is selected, swap contents.
    else if ($drawer.hasClass('open')) {
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
      $drawer.addClass('open');
      $drawerContent.addClass('active');
      $pageWrapper.addClass('drawer-open');
    }
  }
})(jQuery);