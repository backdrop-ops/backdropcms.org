/**
 * Google Analytics Event Tracking
 */
(function($) {
  $(document).ready(function() {
    $('.button.download').on('click', function(){
      var href = this.href;
      ga('send','event','Backdrop','Download',document.title, {'hitCallback': function(){document.location = href;}}); return false;
    });

    $('.button.demo').on('click', function(){
      var href = this.href;
      ga('send','event','Backdrop','Pantheon',document.title, {'hitCallback': function(){document.location = href;}}); return false;
    });

    $('#mc-embedded-subscribe .button').on('click', function(){
      var href = this.href;
      ga('send','event','Newsletter','Subscribe',document.title, {'hitCallback': function(){document.location = href;}}); return false;
    });

    /*
    $('#supporter-node-form .button-primary.form-submit').on('click', function(){
      var href = this.href;
      ga('send','event','Support','Submit',document.title, {'hitCallback': function(){document.location = href;}}); return false;
    });

    $('#contact-site-form .button-primary.form-submit').on('click', function(){
      var href = this.href;
      ga('send','event','Contact','Submit',document.title, {'hitCallback': function(){document.location = href;}}); return false;
    });
    */

    $('#user-register-form .button-primary.form-submit').on('click', function(){
      var href = this.href;
      ga('send','event','User Registration','Submit',document.title, {'hitCallback': function(){document.location = href;}}); return false;
    });
  });
})(jQuery);
