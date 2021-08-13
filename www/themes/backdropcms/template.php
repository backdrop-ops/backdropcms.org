<?php
/**
 * @file
 * Theme and preprocess functions for the backdropcms theme.
 */

/**
 * Prepares variables for page templates.
 * @see page.tpl.php
 */
function backdropcms_preprocess_page(&$variables) {
  $variables['fp'] = '';
  $user_pages = array('login', 'register', 'password');
  if (arg(0) == 'user') {
    if (!in_array(arg(1), $user_pages) && (is_numeric(arg(1)) && arg(2) != 'edit')) {
      $variables['fp'] = "
      <!-- Facebook Pixel Code -->
      <script>
        !function(f,b,e,v,n,t,s)
        {if(f.fbq)return;n=f.fbq=function(){n.callMethod?
        n.callMethod.apply(n,arguments):n.queue.push(arguments)};
        if(!f._fbq)f._fbq=n;n.push=n;n.loaded=!0;n.version='2.0';
        n.queue=[];t=b.createElement(e);t.async=!0;
        t.src=v;s=b.getElementsByTagName(e)[0];
        s.parentNode.insertBefore(t,s)}(window, document,'script',
        'https://connect.facebook.net/en_US/fbevents.js');
        fbq('init', '1687153224763628');
        fbq('track', 'PageView');
      </script>" . '
      <noscript><img height="1" width="1" style="display:none"
        src="https://www.facebook.com/tr?id=1687153224763628&ev=PageView&noscript=1"
      /></noscript>
      <!-- End Facebook Pixel Code -->';
    }
  }
}
