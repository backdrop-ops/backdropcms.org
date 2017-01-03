(function ($) {

"use strict";

$(document).jQuery(document).ready(function($) {
  var $hamburgerToggler = $(
    '<input id="burger-toggler--state" class="burger-toggler--state element-invisible" type="checkbox" aria-controls="">' +
    '<label class="burger-toggler__button" for="burger-toggler--state">' +
      '<span class="burger-toggler__button-icon"></span><span class="burger-toggler__button-text">Menu</span>' +
      '<span class="burger-toggler__assistive-text element-invisible">Toggle main menu visibility</span>' +
    '</label>');

  $('.block-system-main-menu .block-content').prepend($hamburgerToggler);
  console.log('farts');
});


})(jQuery);
