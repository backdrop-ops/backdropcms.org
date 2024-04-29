<?php
/**
 * @file
 * Administrative interface for SecKit settings.
 */

/**
 * Forms administration page.
 */
function seckit_admin_form() {
  $form['#config'] = 'seckit.settings';
  // Get default/configured (and unaltered) options.
  if ($hooks = module_implements('seckit_options_alter')) {
    foreach ($hooks as $key => $module) {
      $hooks[$key] = $module . '_seckit_options_alter()';
    }
    backdrop_set_message(t("Some settings may be overridden at runtime. See @hooks.", array('@hooks' => implode(', ', $hooks))), 'status', FALSE);
    $options = _seckit_get_options(TRUE, FALSE);
  } else {
    $options = _seckit_get_options();
  }
  $defaults = _seckit_get_options_defaults();

  // Main description.
  $args = array(
    '!browserscope' => l(t('Browserscope'), 'https://www.browserscope.org/?category=security'),
  );
  $form['description'] = array(
    '#type' => 'item',
    '#description' => t('This module provides your website with various options to mitigate risks of common web application vulnerabilities like Cross-site Scripting, Cross-site Request Forgery and Clickjacking. It also has some options to improve your SSL/TLS security and fixes Drupal 6 core Upload module issue leading to an easy exploitation of an old Internet Explorer MIME sniffer HTML injection vulnerability. Note that some security features are not supported by all browsers. You may find this out at !browserscope.', $args),
  );

  // Main fieldset for XSS.
  $form['xss'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cross-site Scripting'),
    '#collapsible' => TRUE,
    '#tree' => TRUE,
    '#description' => t('Configure levels and various techniques of protection from cross-site scripting attacks'),
  );

  // Fieldset for Content Security Policy (CSP).
  $args = array(
    '!wiki' => l(t('Mozilla Wiki'), 'https://wiki.mozilla.org/Security/CSP'),
    '!caniuse' => l(t('Can I use'), 'https://caniuse.com/#feat=contentsecuritypolicy'),
  );
  $form['xss']['csp'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content Security Policy'),
    '#collapsible' => TRUE,
    '#collapsed' => !$options['xss']['csp']['checkbox'],
    '#tree' => TRUE,
    '#description' => t('Content Security Policy is a policy framework that allows to specify trustworthy sources of content and to restrict its capabilities. You may read more about it at !wiki.', $args),
  );
  // CSP enable/disable.
  $form['xss']['csp']['checkbox'] = array(
    '#type' => 'checkbox',
    '#default_value' => $options['xss']['csp']['checkbox'],
    '#title' => t('Enable Content Security Policy'),
    '#return_value' => 1,
    '#description' => t('Send Content-Security-Policy HTTP response headers with the list of Content Security Policy directives. <br/>Violation reports are ignored when this option is disabled.'),
  );
  $form['xss']['csp']['vendor-prefix'] = array(
    '#type' => 'fieldset',
    '#title' => t('Vendor Prefixed CSP headers'),
    '#collapsible' => TRUE,
    '#tree' => TRUE,
    '#collapsed' => !($options['xss']['csp']['vendor-prefix']['x'] || $options['xss']['csp']['vendor-prefix']['webkit']),
    '#description' => t('Support for legacy vendor-prefixed CSP headers. Details at !caniuse.', $args),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][checkbox]"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['xss']['csp']['vendor-prefix']['x'] = array(
    '#type' => 'checkbox',
    '#default_value' => $options['xss']['csp']['vendor-prefix']['x'],
    '#title' => t('Send X-Content-Security-Policy HTTP response header'),
    '#return_value' => 1,
    '#description' => t('Send vendor-prefixed X-Content-Security-Policy HTTP response headers with the list of Content Security Policy directives.'),
  );
  $form['xss']['csp']['vendor-prefix']['webkit'] = array(
    '#type' => 'checkbox',
    '#default_value' => $options['xss']['csp']['vendor-prefix']['webkit'],
    '#title' => t('Send X-WebKit-CSP HTTP response header'),
    '#return_value' => 1,
    '#description' => t('Send vendor-prefixed X-WebKit-CSP HTTP response headers with the list of Content Security Policy directives.'),
  );
  // CSP Upgrade Insecure Requests.
  $form['xss']['csp']['upgrade-req'] = array(
    '#type' => 'checkbox',
    '#default_value' => $options['xss']['csp']['upgrade-req'],
    '#title' => t('Enable Upgrade Insecure Requests'),
    '#return_value' => 1,
    '#description' => t("Upgrade Insecure Requests (upgrade-insecure-requests) instructs user agents to rewrite URL schemes, changing HTTP to HTTPS. This directive is used to protect your visitors from insecure content or for websites with large numbers of old URL's that need to be rewritten."),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][checkbox]"]' => array('checked' => TRUE),
      ),
    ),
  );
  // CSP report-only mode.
  $form['xss']['csp']['report-only'] = array(
    '#type' => 'checkbox',
    '#default_value' => $options['xss']['csp']['report-only'],
    '#title' => t('Report Only'),
    '#return_value' => 1,
    '#description' => t('Use Content Security Policy in report-only mode. In this case, violations of policies will only be reported, not blocked. Use this while configuring policies. Reports are logged to watchdog.'),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][checkbox]"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['xss']['csp']['csp_limits'] = array(
    '#type' => 'item',
    '#title' => t("CSP violation reporting limits are currently disabled. See !Advanced_options below.", array('!Advanced_options' => '<a href="#edit-seckit-advanced">' . t('Advanced options') . '</a>')),
    '#description' => t("Reporting limits should be enabled once your CSP is production-ready, to prevent excessive report logging should the violation report URL be flooded."),
    '#states' => array(
      'visible' => array(
        'input[name="seckit_advanced[unlimited_csp_reports]"]' => array('checked' => TRUE),
        'input[name="xss[csp][checkbox]"]' => array('checked' => TRUE),
      ),
    ),
  );
  // CSP description.
  $args = array(
    '!keywords' => theme('item_list', array(
      'items' => array(
        "'none' - block content from any source",
        "'self' - allow content only from your domain",
        "'unsafe-inline' - allow specific inline content (note, that it is supported by a subset of directives)",
        "'unsafe-eval' - allow a set of string-to-code API which is restricted by default (supported by script-src directive)",
      ),
    )),
    '!wildcards' => theme('item_list', array(
      'items' => array(
        '* - load content from any source',
        '*.example.com - load content from example.com and all its subdomains',
        'example.com:* - load content from example.com via any port.  Otherwise, it will use your website default port',
      ),
    )),
    '!spec' => l(t('specification page'), 'https://www.w3.org/TR/CSP/'),
  );
  $form['xss']['csp']['directives'] = array(
    '#type' => 'fieldset',
    '#title' => t('Directives'),
    '#collapsible' => TRUE,
    '#tree' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t("Set up security policy for different types of content. Don't use www prefix. Keywords are: !keywords Wildcards (*) are allowed: !wildcards More information is available at !spec.", $args),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][checkbox]"]' => array('checked' => TRUE),
      ),
    ),
  );
  // Use a policy-uri checkbox toggle.
  $form['xss']['csp']['directives']['use-policy-uri'] = array(
    '#type' => 'checkbox',
    '#default_value' => $options['xss']['csp']['directives']['use-policy-uri'],
    '#title' => t('Use a policy-uri directive'),
    '#return_value' => 1,
    '#description' => t('When using policy-uri, <strong>all other directives will be omitted</strong> by Security Kit, as <code>policy-uri</code> may only be defined in the <em>absence</em> of other policy definitions in the <code>X-Content-Security-Policy</code> HTTP header.'),
  );
  // CSP policy-uri directive.
  $form['xss']['csp']['directives']['policy-uri'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['xss']['csp']['directives']['policy-uri'],
    '#title' => 'policy-uri',
    '#description' => t("Specify a URL (relative to the Backdrop root) for a file containing the (entire) policy. The MIME type for this URI <strong>must</strong> be <code>text/x-content-security-policy</code>, otherwise user-agents will enforce the policy <code>allow 'none'</code>  instead."),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => TRUE),
      ),
      'required' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => TRUE),
      ),
    ),
  );
  // CSP default-src directive.
  $form['xss']['csp']['directives']['default-src'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['xss']['csp']['directives']['default-src'],
    '#title' => 'default-src',
    '#description' => t("Specify security policy for all types of content, which are not specified further (frame-ancestors excepted). Default is 'self'."),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => FALSE),
      ),
    ),
  );
  // CSP script-src directive.
  $form['xss']['csp']['directives']['script-src'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['xss']['csp']['directives']['script-src'],
    '#title' => 'script-src',
    '#description' => t('Specify trustworthy sources for &lt;script&gt; elements.'),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => FALSE),
      ),
    ),
  );
  // CSP script-src-elem directive.
  $form['xss']['csp']['directives']['script-src-elem'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['xss']['csp']['directives']['script-src-elem'],
    '#title' => 'script-src-elem',
    '#description' => t('Specify trustworthy sources for elements in &lt;script&gt; elements.'),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => FALSE),
      ),
    ),
  );
  // CSP object-src directive.
  $form['xss']['csp']['directives']['object-src'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['xss']['csp']['directives']['object-src'],
    '#title' => 'object-src',
    '#description' => t('Specify trustworthy sources for &lt;object&gt;, &lt;embed&gt; and &lt;applet&gt; elements.'),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => FALSE),
      ),
    ),
  );
  // CSP style-src directive.
  $form['xss']['csp']['directives']['style-src'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['xss']['csp']['directives']['style-src'],
    '#title' => 'style-src',
    '#description' => t('Specify trustworthy sources for stylesheets. Note, that inline stylesheets and style attributes of HTML elements are allowed.'),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => FALSE),
      ),
    ),
  );
  // CSP style-src-elem directive.
  $form['xss']['csp']['directives']['style-src-elem'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['xss']['csp']['directives']['style-src-elem'],
    '#title' => 'style-src-elem',
    '#description' => t('Specify trustworthy sources for elements in stylesheets. Note, that inline stylesheets and style attributes of HTML elements are allowed.'),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => FALSE),
      ),
    ),
  );
  // CSP img-src directive.
  $form['xss']['csp']['directives']['img-src'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['xss']['csp']['directives']['img-src'],
    '#title' => 'img-src',
    '#description' => t('Specify trustworthy sources for &lt;img&gt; elements.'),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => FALSE),
      ),
    ),
  );
  // CSP media-src directive.
  $form['xss']['csp']['directives']['media-src'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['xss']['csp']['directives']['media-src'],
    '#title' => 'media-src',
    '#description' => t('Specify trustworthy sources for &lt;audio&gt; and &lt;video&gt; elements.'),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => FALSE),
      ),
    ),
  );
  // CSP frame-src directive.
  $form['xss']['csp']['directives']['frame-src'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['xss']['csp']['directives']['frame-src'],
    '#title' => 'frame-src',
    '#description' => t('Specify trustworthy sources for &lt;iframe&gt; and &lt;frame&gt; elements.'),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => FALSE),
      ),
    ),
  );
  // CSP frame-ancestors directive.
  $form['xss']['csp']['directives']['frame-ancestors'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['xss']['csp']['directives']['frame-ancestors'],
    '#title' => 'frame-ancestors',
    '#description' => t("Specify trustworthy hosts which are allowed to embed this site's resources via &lt;iframe&gt;, &lt;frame&gt;, &lt;object&gt;, &lt;embed&gt; and &lt;applet&gt; elements."),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => FALSE),
      ),
    ),
  );
  // CSP child-src directive.
  $form['xss']['csp']['directives']['child-src'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['xss']['csp']['directives']['child-src'],
    '#title' => 'child-src',
    '#description' => t('Specify trustworthy sources for &lt;iframe&gt; and &lt;frame&gt; elements as well as for loading Workers.'),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => FALSE),
      ),
    ),
  );
  // CSP font-src directive.
  $form['xss']['csp']['directives']['font-src'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['xss']['csp']['directives']['font-src'],
    '#title' => 'font-src',
    '#description' => t('Specify trustworthy sources for @font-src CSS loads.'),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => FALSE),
      ),
    ),
  );
  // CSP connect-src directive.
  $form['xss']['csp']['directives']['connect-src'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['xss']['csp']['directives']['connect-src'],
    '#title' => 'connect-src',
    '#description' => t('Specify trustworthy sources for XMLHttpRequest, WebSocket and EventSource connections.'),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => FALSE),
      ),
    ),
  );
  // CSP worker-src directive.
  $form['xss']['csp']['directives']['worker-src'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['xss']['csp']['directives']['worker-src'],
    '#title' => 'worker-src',
    '#description' => t('Specify trustworthy sources for Worker, SharedWorker, or ServiceWorker scripts.'),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => FALSE),
      ),
    ),
  );
  // CSP report-uri directive.
  $form['xss']['csp']['directives']['report-uri'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['xss']['csp']['directives']['report-uri'],
    '#title' => 'report-uri',
    '#description' => t('Specify a URL (relative to the Backdrop root or absolute) to which user-agents will report CSP violations. Use the default value of <code>@csp_report_url</code> (which logs the report data in watchdog), unless you have set up an alternative handler for these reports.', array('@csp_report_url' => SECKIT_CSP_REPORT_URL)),
    '#states' => array(
      'visible' => array(
        'input[name="xss[csp][directives][use-policy-uri]"]' => array('checked' => FALSE),
      ),
    ),
  );

  // Fieldset for X-XSS-Protection.
  $form['xss']['x_xss'] = array(
    '#type' => 'fieldset',
    '#title' => t('X-XSS-Protection header'),
    '#collapsible' => TRUE,
    '#collapsed' => ($options['xss']['x_xss']['select'] == SECKIT_X_XSS_DISABLE),
    '#tree' => TRUE,
    '#description' => t('X-XSS-Protection HTTP response header controls Microsoft Internet Explorer, Google Chrome and Apple Safari internal XSS filters.'),
  );
  // Options for X-XSS-Protection.
  $x_xss_protection_options = array(
    SECKIT_X_XSS_DISABLE => t('Disabled'),
    SECKIT_X_XSS_0 => '0',
    SECKIT_X_XSS_1 => '1',
    SECKIT_X_XSS_1_BLOCK => '1; mode=block',
  );
  // Configure X-XSS-Protection.
  $link = l(t("IE's XSS filter security flaws in past"), 'https://hackademix.net/2009/11/21/ies-xss-filter-creates-xss-vulnerabilities');
  $args = array(
    '!values' => theme('item_list', array(
      'items' => array(
        'Disabled - XSS filter will work in default mode. Enabled by default',
        '0 - XSS filter will be disabled for a website. It may be useful because of ' . $link,
        '1 - XSS filter will be left enabled, and will modify dangerous content',
        '1; mode=block - XSS filter will be left enabled, but it will block entire page instead of modifying dangerous content',
      ),
    )),
  );
  $form['xss']['x_xss']['select'] = array(
    '#type' => 'select',
    '#title' => t('Configure'),
    '#options' => $x_xss_protection_options,
    '#default_value' => $options['xss']['x_xss']['select'],
    '#description' => t('!values', $args),
  );

  // Main fieldset for CSRF.
  $form['csrf'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cross-site Request Forgery'),
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => !$options['csrf']['origin'],
    '#description' => t('Configure levels and various techniques of protection from cross-site request forgery attacks'),
  );
  // Enable/disable Origin.
  $form['csrf']['origin'] = array(
    '#type' => 'checkbox',
    '#title' => t('HTTP Origin'),
    '#default_value' => $options['csrf']['origin'],
    '#description' => t('Check Origin HTTP request header.'),
  );
  // Origin whitelist.
  $form['csrf']['origin_whitelist'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#title' => t('Allow requests from'),
    '#default_value' => implode(", ", $options['csrf']['origin_whitelist']),
    '#description' => t('Comma separated list of trustworthy sources. Do not enter your website URL - it is automatically added. Syntax of the source is: [protocol] :// [host] : [port] . E.g, http://example.com, https://example.com, https://www.example.com, http://www.example.com:8080'),
    '#states' => array(
      'visible' => array(
        'input[name="csrf[origin]"]' => array('checked' => TRUE),
      ),
    ),
  );

  // Main fieldset for Clickjacking.
  $form['clickjacking'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clickjacking'),
    '#collapsible' => TRUE,
    '#tree' => FALSE,
    '#description' => t('Configure levels and various techniques of protection from Clickjacking/UI Redressing attacks'),
  );

  // Fieldset for X-Frame-Options settings. non-#tree.
  $form['clickjacking']['x_frame_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('X-Frame-Options header'),
    '#collapsible' => TRUE,
    '#collapsed' => ($options['clickjacking']['x_frame'] == SECKIT_X_FRAME_DISABLE),
    '#tree' => FALSE,
    '#description' => t('Configure the X-Frame-Options HTTP header'),
  );

  // Options for X-Frame-Options.
  $x_frame_options = array(
    SECKIT_X_FRAME_DISABLE => t('Disabled'),
    SECKIT_X_FRAME_SAMEORIGIN => 'SAMEORIGIN',
    SECKIT_X_FRAME_DENY => 'DENY',
    SECKIT_X_FRAME_ALLOW_FROM => 'ALLOW-FROM',
  );
  // Configure X-Frame-Options.
  $args = array(
    '!values' => theme('item_list', array(
      'items' => array(
        'Disabled - turn off X-Frame-Options',
        'SAMEORIGIN - browser allows all the attempts of framing website within its domain. Enabled by default',
        'DENY - browser rejects any attempt of framing website',
        'ALLOW-FROM - browser allows framing website only from specified source',
      ),
    )),
    '!msdn' => l(t('MSDN article'), 'https://blogs.msdn.com/b/ie/archive/2009/01/27/ie8-security-part-vii-clickjacking-defenses.aspx'),
    '!spec' => l(t('specification'), 'https://tools.ietf.org/html/draft-ietf-websec-x-frame-options-01'),
  );
  $form['clickjacking']['x_frame_options']['x_frame'] = array(
    '#type' => 'select',
    '#title' => t('X-Frame-Options'),
    '#options' => $x_frame_options,
    '#default_value' => $options['clickjacking']['x_frame'],
    '#description' => t("X-Frame-Options HTTP response header controls browser's policy of frame rendering. Possible values: !values You may read more about it at !msdn or !spec.", $args),
    // Non-tree (we skip a parent).
    '#parents' => array(
      'clickjacking',
      'x_frame',
    ),
  );

  // Origin value for "ALLOW-FROM" option.
  $form['clickjacking']['x_frame_options']['x_frame_allow_from'] = array(
    '#type' => 'textarea',
    '#title' => t('ALLOW-FROM'),
    '#default_value' => implode("\n", $options['clickjacking']['x_frame_allow_from']),
    '#description' => t('Origin URIs (as specified by RFC 6454) for the "X-Frame-Options: ALLOW-FROM" value. One per line. Example, https://domain.com'),
    // Non-tree (we skip a parent).
    '#parents' => array(
      'clickjacking',
      'x_frame_allow_from',
    ),
    '#states' => array(
      'visible' => array(
        'select[name="clickjacking[x_frame]"]' => array('value' => SECKIT_X_FRAME_ALLOW_FROM),
      ),
      'required' => array(
        'select[name="clickjacking[x_frame]"]' => array('value' => SECKIT_X_FRAME_ALLOW_FROM),
      ),
    ),
  );

  // Fieldset for JavaScript settings. non-#tree.
  $form['clickjacking']['javascript'] = array(
    '#type' => 'fieldset',
    '#title' => t('JavaScript-based protection'),
    '#collapsible' => TRUE,
    '#collapsed' => !$options['clickjacking']['js_css_noscript'],
    '#tree' => FALSE,
    '#description' => t("Warning: With this enabled, the site <em>will not work at all</em> for users who have JavaScript disabled (e.g. users running the popular !NoScript browser extension, if they haven't whitelisted your site).", array('!NoScript' => l(t('NoScript'), 'https://noscript.net/'))),
  );

  // Enable/disable JS + CSS + Noscript protection.
  $args = array(
    '!EduardoVela' => l(t('Eduardo Vela'), 'https://sirdarckcat.blogspot.com/'),
    '%js' => 'seckit.document_write.js',
    '%stop' => '<!-- stop SecKit protection -->',
    '%css' => 'seckit.no_body.css',
  );
  $form['clickjacking']['javascript']['js_css_noscript'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable JavaScript + CSS + Noscript protection'),
    '#return_value' => 1,
    '#default_value' => $options['clickjacking']['js_css_noscript'],
    '#description' => t('Enable protection via JavaScript, CSS and &lt;noscript&gt; tag. This is the most efficient Clickjacking prevention technique. If website is not being framed, %js starts commenting with <em>document.write()</em> and stops when the comment %stop is reached. Thus %css, which hides the page body, is ignored. If particularly this JavaScript file is being blocked (with XSS filter of Internet Explorer 8 or Safari), %css applies <em>display: none</em> to <em>body</em>, hiding it. If JavaScript is disabled within browser, it shows a special message. Credits for this trick go to !EduardoVela.', $args),
    // Non-tree (we skip a parent).
    '#parents' => array(
      'clickjacking',
      'js_css_noscript',
    ),
  );

  // Custom text for "disabled JavaScript" message.
  $form['clickjacking']['javascript']['noscript_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Custom text for disabled JavaScript message'),
    '#default_value' => $options['clickjacking']['noscript_message'],
    '#description' => t('This message will be shown to user when JavaScript is disabled or unsupported in his browser. Default is "Sorry, you need to enable JavaScript to visit this website."'),
    // Non-tree (we skip a parent).
    '#parents' => array(
      'clickjacking',
      'noscript_message',
    ),
    '#states' => array(
      'visible' => array(
        'input[name="clickjacking[js_css_noscript]"]' => array('checked' => TRUE),
      ),
      'required' => array(
        'input[name="clickjacking[js_css_noscript]"]' => array('checked' => TRUE),
      ),
    ),
  );

  // Main fieldset for SSL/TLS.
  $form['ssl'] = array(
    '#type' => 'fieldset',
    '#title' => t('SSL/TLS'),
    '#collapsible' => TRUE,
    '#collapsed' => !$options['ssl']['hsts'],
    '#tree' => TRUE,
    '#description' => t('Configure various techniques to improve security of SSL/TLS'),
  );

  // enable/disable HTTP Strict Transport Security (HSTS)
  $args = array(
    '!wiki' => l(t('Wikipedia'), 'https://en.wikipedia.org/wiki/HTTP_Strict_Transport_Security'),
  );
  $form['ssl']['hsts'] = array(
    '#type' => 'checkbox',
    '#title' => t('HTTP Strict Transport Security'),
    '#description' => t('Enable Strict-Transport-Security HTTP response header. HTTP Strict Transport Security (HSTS) header is proposed to prevent eavesdropping and man-in-the-middle attacks like SSLStrip, when a single non-HTTPS request is enough for credential theft or hijacking. It forces browser to connect to the server in HTTPS-mode only and automatically convert HTTP links into secure before sending request. !wiki has more information about HSTS', $args),
    '#default_value' => $options['ssl']['hsts'],
    '#return_value' => 1,
  );
  // HSTS max-age directive.
  $form['ssl']['hsts_max_age'] = array(
    '#type' => 'textfield',
    '#title' => t('Max-Age'),
    '#description' => t('Specify Max-Age value in seconds. It sets period when user-agent should remember receipt of this header field from this server. Default is 1000.'),
    '#default_value' => $options['ssl']['hsts_max_age'],
    '#states' => array(
      'visible' => array(
        'input[name="ssl[hsts]"]' => array('checked' => TRUE),
      ),
      'required' => array(
        'input[name="ssl[hsts]"]' => array('checked' => TRUE),
      ),
    ),
  );
  // HSTS includeSubDomains directive.
  $form['ssl']['hsts_subdomains'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include Subdomains'),
    '#description' => t('Force HTTP Strict Transport Security for all subdomains. If enabled, HSTS policy will be applied for all subdomains, otherwise only for the main domain.'),
    '#return_value' => 1,
    '#default_value' => $options['ssl']['hsts_subdomains'],
    '#states' => array(
      'visible' => array(
        'input[name="ssl[hsts]"]' => array('checked' => TRUE),
      ),
    ),
  );
  $args = array(
    '!HSTS_preload_list' => l(t('HSTS Preload list'), 'https://hstspreload.appspot.com/'),
  );
  // HSTS preload directive.
  $form['ssl']['hsts_preload'] = array(
    '#type' => 'checkbox',
    '#title' => t('Preload'),
    '#description' => t("If you intend to submit your domain to the !HSTS_preload_list, you will need to enable the preload flag as confirmation. Don't submit your domain unless you're sure that you can support HTTPS for the long term, as this action cannot be undone.", $args),
    '#return_value' => 1,
    '#default_value' => $options['ssl']['hsts_preload'],
    '#states' => array(
      'visible' => array(
        'input[name="ssl[hsts]"]' => array('checked' => TRUE),
      ),
    ),
  );

  // Main fieldset for Expect-CT.
  $args = array(
    '!url_docs' => "<a href='https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Expect-CT'>Mozilla's developer documentation</a>.",
  );
  $form['ct'] = array(
    '#type' => 'fieldset',
    '#title' => t('Expect-CT'),
    '#collapsible' => TRUE,
    '#collapsed' => !$options['ct']['expect_ct'],
    '#tree' => TRUE,
    '#description' => t('Configure the Expect-CT header which allows sites to opt in to reporting and/or enforcement of Certificate Transparency requirements. See !url_docs', $args),
  );

  // enable/disable the Expect-CT settings.
  $form['ct']['expect_ct'] = array(
    '#type' => 'checkbox',
    '#title' => t('Expect-CT'),
    '#description' => t('Enable the Expect-CT header.'),
    '#default_value' => $options['ct']['expect_ct'],
    '#return_value' => 1,
  );
  // Expect-CT max-age directive.
  $form['ct']['max_age'] = array(
    '#type' => 'textfield',
    '#title' => t('Max-Age'),
    '#description' => t('Specify Max-Age value in seconds.'),
    '#default_value' => $options['ct']['max_age'],
    '#states' => array(
      'required' => array(
        'input[name="seckit_ct[expect_ct]"]' => array('checked' => TRUE),
      ),
    ),
  );
  // Expect-CT report-uri directive.
  $form['ct']['report-uri'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#default_value' => $options['ct']['report-uri'],
    '#title' => 'report-uri',
    '#description' => t('Specify the (absolute) URI to which the user agent should report Expect-CT failures.'),
  );
  // Expect-CT enforce directive.
  $form['ct']['enforce'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enforce'),
    '#description' => t('Enforce the Certificate Transparency policy.'),
    '#return_value' => 1,
    '#default_value' => $options['ct']['enforce'],
  );

  // Main fieldset for feature_policy.
  $args = array(
    '!Google' => l(t("Google's developer documentation"), 'https://developers.google.com/web/updates/2018/06/feature-policy'),
  );
  $form['fp'] = array(
    '#type' => 'fieldset',
    '#title' => t('Feature policy'),
    '#collapsible' => TRUE,
    '#collapsed' => (!$options['fp']['feature_policy']),
    '#tree' => TRUE,
    '#description' => t("Allows configuration of the Feature-Policy header to selectively enable, disable, and modify the behavior of certain APIs and web features in the browser. See !Google.", $args),
  );

  // Enable/disable Feature Policy.
  $form['fp']['feature_policy'] = array(
    '#type' => 'checkbox',
    '#title' => t('Feature policy'),
    '#default_value' => $options['fp']['feature_policy'],
    '#description' => t('Enable the Feature-Policy header.'),
  );
  // From-Origin destination.
  $form['fp']['feature_policy_policy'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#title' => t('Policy'),
    '#default_value' => $options['fp']['feature_policy_policy'],
    '#description' => t('Specify the policy to be sent out with Feature-Policy headers.'),
  );

  // Main fieldset for various.
  $form['various'] = array(
    '#type' => 'fieldset',
    '#title' => t('Miscellaneous'),
    '#collapsible' => TRUE,
    '#collapsed' => ($options['various'] == $defaults['various']),
    '#tree' => TRUE,
    '#description' => t('Configure miscellaneous other security enhancements'),
  );

  // Enable/disable Referrer-Policy.
  $args = array(
    '!Referrer_Policy_Spec' => l(t('specification'), 'https://www.w3.org/TR/referrer-policy/'),
  );

  $form['various']['referrer_policy'] = array(
    '#type' => 'checkbox',
    '#title' => t('Referrer-Policy'),
    '#default_value' => $options['various']['referrer_policy'],
    '#description' => t('Enable Referrer-Policy HTTP response header. This affects the Referer HTTP header for outgoing requests and navigations. More information is available at the !Referrer_Policy_Spec page.', $args),
  );

  // Referrer-Policy policy selection.
  $items = array(
    t('no-referrer - No referrer information will be sent.'),
    t('no-referrer-when-downgrade - No referrer information will be sent when navigating from HTTPS to HTTP.'),
    t('same-origin - Referrer information is sent only with requests to the same origin.'),
    t('origin - This will strip any path information from the referrer information.'),
    t('strict-origin - Like no-referrer-when-downgrade, but sends only the origin.'),
    t('origin-when-cross-origin - Sends full referrer information with same-origin request and origin only with cross-origin requests.'),
    t('strict-origin-when-cross-origin - Sends full referrer information with same-origin requests, origin only with cross-origin requests, and no referrer information if downgrading from https to http.'),
    t('unsafe-url - Sends full referrer information with all requests; as the name implies, this is not safe.'),
    t('"" (empty string) - Default to no-referrer-when-downgrade unless a higher-level policy has been defined elsewhere.'),
  );
  $args = array(
    '!items' => theme('item_list', array('items' => $items)),
  );

  $referrer_policy_options = array(
    'no-referrer' => 'no-referrer',
    'no-referrer-when-downgrade' => 'no-referrer-when-downgrade',
    'same-origin' => 'same-origin',
    'strict-origin' => 'strict-origin',
    'origin-when-cross-origin' => 'origin-when-cross-origin',
    'strict-origin-when-cross-origin' => 'strict-origin-when-cross-origin',
    'unsafe-url' => 'unsafe-url',
    'empty' => t('"" (empty string)'),
  );
  $form['various']['referrer_policy_policy'] = array(
    '#type' => 'select',
    '#title' => t('Select policy'),
    '#default_value' => $options['various']['referrer_policy_policy'],
    '#description' => t('Policy options are: !items', $args),
    '#options' => $referrer_policy_options,
    '#states' => array(
      'required' => array(
        'input[name="seckit_various[referrer_policy]"]' => array('checked' => TRUE),
      ),
    ),
  );

  // Enable/disable From-Origin.
  $args = array(
    '!spec' => l(t('specification'), 'https://www.w3.org/TR/from-origin/'),
  );
  $form['various']['from_origin'] = array(
    '#type' => 'checkbox',
    '#title' => t('From-Origin'),
    '#default_value' => $options['various']['from_origin'],
    '#description' => t('Enable From-Origin HTTP response header. This forces user-agent to retrieve embedded content from your site only to listed destination. More information is available at !spec page.', $args),
  );
  // From-Origin destination.
  $args = array(
    '!items' => theme('item_list', array(
      'items' => array(
        'same - allow loading of content only from your site. Default value.',
        'serialized origin - address of trustworthy destination. For example, http://example.com, https://example.com, https://www.example.com, http://www.example.com:8080',
      ),
    )),
  );
  $form['various']['from_origin_destination'] = array(
    '#type' => 'textarea',
    '#rows' => 1,
    '#title' => t('Allow loading content to'),
    '#default_value' => $options['various']['from_origin_destination'],
    '#description' => t('Trustworthy destination. Possible variants are: !items', $args),
    '#states' => array(
      'visible' => array(
        'input[name="various[from_origin]"]' => array('checked' => TRUE),
      ),
      'required' => array(
        'input[name="various[from_origin]"]' => array('checked' => TRUE),
      ),
    ),
  );
  // Disable autocomplete on login and registration forms.
  $form['various']['disable_autocomplete'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable autocomplete on login and registration forms'),
    '#default_value' => $options['various']['disable_autocomplete'],
    '#description' => t('Prevent the browser from populating login/registration form fields using its autocomplete functionality. This as populated fields may contain sensitive information, facilitating unauthorized access.'),
  );

  // Advanced / developer options.
  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced options'),
    '#collapsible' => TRUE,
    '#collapsed' => ($options['advanced'] == $defaults['advanced']),
    '#tree' => TRUE,
    // '#description' => t(''),
  );
  $form['advanced']['disable_seckit'] = array(
    '#type' => 'checkbox',
    '#default_value' => $options['advanced']['disable_seckit'],
    '#title' => t('Disable Security Kit'),
    '#description' => t('Prevent the module from doing anything.'),
  );
  if ($options['advanced']['disable_seckit'] && empty($_POST)) {
    backdrop_set_message(t("Security Kit is currently disabled in the Advanced options (below)."), 'warning');
  }

  // CSP report limits.
  $form['advanced']['unlimited_csp_reports'] = array(
    '#type' => 'checkbox',
    '#default_value' => $options['advanced']['unlimited_csp_reports'],
    '#title' => t('Unlimited CSP reports'),
    '#description' => t('Ignore restrictions on the size and quantity of CSP violation reports. This should be disabled once the CSP is production-ready.'),
  );
  $form['advanced']['csp_limits'] = array(
    '#type' => 'fieldset',
    '#title' => t('CSP violation reporting limits'),
    '#description' => t("Reports breaching these limits will not be logged."),
    '#collapsible' => TRUE,
    '#collapsed' => ($options['advanced']['csp_limits'] == $defaults['advanced']['csp_limits']),
    '#states' => array(
      'visible' => array(
        'input[name="seckit_advanced[unlimited_csp_reports]"]' => array('checked' => FALSE),
      ),
    ),
  );
  $limits = $options['advanced']['csp_limits'];
  $form['advanced']['csp_limits']['max_size'] = array(
    '#title' => t('Maximum report size (bytes)'),
    '#type' => 'textfield',
    '#maxlength' => 16,
    '#default_value' => $limits['max_size'],
  );
  $form['advanced']['csp_limits']['flood'] = array(
    '#type' => 'fieldset',
    '#title' => t('Flood settings'),
    '#collapsible' => TRUE,
    '#collapsed' => ($options['advanced']['csp_limits']['flood'] == $defaults['advanced']['csp_limits']['flood']),
  );
  $form['advanced']['csp_limits']['flood']['limit_user'] = array(
    '#title' => t('Maximum reports per IP address'),
    '#description' => t('Applicable within the given time window'),
    '#type' => 'textfield',
    '#maxlength' => 16,
    '#default_value' => $limits['flood']['limit_user'],
  );
  $form['advanced']['csp_limits']['flood']['window_user'] = array(
    '#title' => t('Time window for per IP address flood detection'),
    '#description' => t('Duration in seconds'),
    '#type' => 'textfield',
    '#maxlength' => 16,
    '#default_value' => $limits['flood']['window_user'],
  );
  $form['advanced']['csp_limits']['flood']['limit_global'] = array(
    '#title' => t('Maximum reports globally (i.e. irrespective of IP address)'),
    '#description' => t('Applicable within the given time window'),
    '#type' => 'textfield',
    '#maxlength' => 16,
    '#default_value' => $limits['flood']['limit_global'],
  );
  $form['advanced']['csp_limits']['flood']['window_global'] = array(
    '#title' => t('Time window for global flood detection'),
    '#description' => t('Duration in seconds'),
    '#type' => 'textfield',
    '#maxlength' => 16,
    '#default_value' => $limits['flood']['window_global'],
  );

  // Execute the default handlers first.
  $form = system_settings_form($form);

  // Run custom validation and submission handlers afterwards.
  $form['#validate'][] = 'seckit_admin_form_validate';
  $form['#submit'][] = 'seckit_admin_form_submit';

  $form['#attached']['css'] = array(
    backdrop_get_path('module', 'seckit') . '/css/seckit.admin.css',
  );

  return $form;
}

/**
 * Validates form data.
 */
function seckit_admin_form_validate($form, &$form_state) {
  // Check validity of CSP report-uri when CSP headers are on.
  $is_csp_header_enabled = $form_state['values']['xss']['csp']['checkbox'];
  $report_uri = $form_state['values']['xss']['csp']['directives']['report-uri'];
  // Ignore empty report-uri as it will be filled in by the default later.
  if ($is_csp_header_enabled && !empty($report_uri) && !backdrop_valid_path($report_uri)) {
    form_error($form['xss']['csp']['directives']['report-uri'], t('Non-existent path for report-uri given: %uri', array('%uri' => $report_uri)));
  }
  // If From-Origin is enabled, it should be explicitly set.
  $from_origin_enable = $form_state['values']['various']['from_origin'];
  $from_origin_destination = $form_state['values']['various']['from_origin_destination'];
  if (($from_origin_enable == 1) && (!$from_origin_destination)) {
    form_error($form['various']['from_origin_destination'], t('You have to set up trustworthy destination for From-Origin HTTP response header. Default is same.'));
  }
  // If X-Frame-Options is set to ALLOW-FROM, it should be explicitly set.
  $x_frame_value = $form_state['values']['clickjacking']['x_frame'];
  if ($x_frame_value == SECKIT_X_FRAME_ALLOW_FROM) {
    $x_frame_allow_from = $form_state['values']['clickjacking']['x_frame_allow_from'];
    if (!_seckit_explode_value($x_frame_allow_from)) {
      form_error($form['clickjacking']['x_frame_allow_from'], t('You must specify a trusted Origin for the ALLOW-FROM value of the X-Frame-Options HTTP response header.'));
    }
  }
  // If HTTP Strict Transport Security is enabled, max-age must be specified.
  // HSTS max-age should only contain digits.
  $hsts_enable = $form_state['values']['ssl']['hsts'];
  $hsts_max_age = $form_state['values']['ssl']['hsts_max_age'];
  if (($hsts_enable == 1) && (!$hsts_max_age)) {
    form_error($form['ssl']['hsts_max_age'], t('You have to set up Max-Age value for HTTP Strict Transport Security. Default is 1000.'));
  }
  if (preg_match('/[^0-9]/', $hsts_max_age)) {
    form_error($form['ssl']['hsts_max_age'], t('Only digits are allowed in HTTP Strict Transport Security Max-Age field.'));
  }
  // If JS + CSS + Noscript Clickjacking protection is enabled,
  // custom text for disabled JS must be specified.
  $js_css_noscript_enable = $form_state['values']['clickjacking']['js_css_noscript'];
  $noscript_message = $form_state['values']['clickjacking']['noscript_message'];
  if (($js_css_noscript_enable == 1) && (!$noscript_message)) {
    form_error($form['clickjacking']['noscript_message'], t('You have to set up Custom text for disabled JavaScript message when JS + CSS + Noscript protection is enabled.'));
  }

  // Check for newlines in some textarea inputs where there should be none.
  $csp_textareas = array(
    'default-src',
    'script-src',
    'script-src-elem',
    'object-src',
    'style-src',
    'style-src-elem',
    'img-src',
    'media-src',
    'frame-src',
    'frame-ancestors',
    'child-src',
    'font-src',
    'connect-src',
    'worker-src',
    'report-uri',
    'policy-uri',
  );
  foreach ($csp_textareas as $csp_textarea) {
    $value = $form_state['values']['xss']['csp']['directives'][$csp_textarea];
    if ($value !== str_replace(array("\r", "\n"), '', (string) $value)) {
      form_error($form['xss']['csp']['directives'][$csp_textarea], t('CSP directives cannot contain newlines.'));
    }
  }
  $value = $form_state['values']['fp']['feature_policy_policy'];
  if ($value !== str_replace(array("\r", "\n"), '', (string) $value)) {
    form_error($form['fp']['feature_policy_policy'], t('Feature policy cannot contain newlines.'));
  }
  $value = $form_state['values']['ct']['report-uri'];
  if ($value !== str_replace(array("\r", "\n"), '', (string) $value)) {
    form_error($form['ct']['report-uri'], t('Expect-CT report-uri cannot contain newlines.'));
  }
  $value = $form_state['values']['csrf']['origin_whitelist'];
  if ($value !== str_replace(array("\r", "\n"), '', (string) $value)) {
    form_error($form['csrf']['origin_whitelist'], t('CSRF Origin Whitelist cannot contain newlines.'));
  }
}

/**
 * Submits form.
 */
function seckit_admin_form_submit($form, &$form_state) {
  // Ensure the core 'x_frame_options' setting matches.
  $x_frame_option = (!empty($form_state['values']['clickjacking']['x_frame'])) ? $form_state['values']['clickjacking']['x_frame'] : '';
  config_set('system.core', 'x_frame_options', $x_frame_option);
  // Our dependency on seckit_boot() may have changed, so reset the
  // bootstrap module settings and cache.
  _system_update_bootstrap_status();
}
