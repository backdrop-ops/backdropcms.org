<?php
/**
 * @file
 */

/**
 * Implements hook_metatag_config_instance_info().
 */
function metatag_metatag_config_instance_info() {
  // @todo remove these once metatag_config_instance_label() is gone.
  $info['global']           = array('label' => t('Global'));
  $info['global:frontpage'] = array('label' => t('Home page'));
  $info['global:403']       = array('label' => t('403 access denied'));
  $info['global:404']       = array('label' => t('404 page not found'));

  // Add instance information for entities.
  $entity_types = entity_get_info();
  foreach ($entity_types as $entity_type => $entity_info) {
    if (metatag_entity_supports_metatags($entity_type)) {
      $info[$entity_type] = array('label' => $entity_info['label']);
      foreach ($entity_info['bundles'] as $bundle => $bundle_info) {
        if (count($entity_info['bundles']) == 1 && $bundle == $entity_type) {
          // Skip default bundles (entities that do not really have bundles).
          continue;
        }
        if (metatag_entity_supports_metatags($entity_type, $bundle)) {
          $info[$entity_type . ':' . $bundle] = array('label' => $bundle_info['label']);
        }
      }
    }
  }

  return $info;
}

/**
 * Implements hook_metatag_info().
 */
function metatag_metatag_info() {
  $info['groups']['basic'] = array(
    'label' => t('Basic tags'),
    'form' => array(
      '#weight' => 1,
      '#collapsed' => FALSE,
    ),
  );
  $info['groups']['advanced'] = array(
    'label' => t('Advanced tags'),
    'form' => array(
      '#weight' => 2,
    ),
  );

  // "Simple" meta tags go first.
  $weight = 0;

  $info['tags']['title'] = array(
    'label' => t('Page title'),
    'description' => t("The text to display in the title bar of web browsers. May also be used as the title for a bookmark."),
    'class' => 'BackdropTitleMetaTag',
    'group' => 'basic',
    'weight' => ++$weight,
  );

  $info['tags']['description'] = array(
    'label' => t('Description'),
    'description' => t("A concise summary of the page's content, preferably 150 characters or less. May be used by in search results."),
    'class' => 'BackdropTextMetaTag',
    'group' => 'basic',
    'weight' => ++$weight,
    'form' => array(
      '#type' => 'textarea',
      '#rows' => 2,
      '#wysiwyg' => FALSE,
    ),
  );

  $info['tags']['abstract'] = array(
    'label' => t('Abstract'),
    'description' => t("A concise summary of the page's content, preferably 150 characters or less. May be used to archive a summary about the page. This meta tag is <em>no longer</em> supported by major search engines."),
    'class' => 'BackdropTextMetaTag',
    'group' => 'basic',
    'weight' => ++$weight,
    'form' => array(
      '#type' => 'textarea',
      '#rows' => 2,
      '#wysiwyg' => FALSE,
    ),
  );

  $info['tags']['keywords'] = array(
    'label' => t('Keywords'),
    'description' => t("A comma-separated list of keywords about the page. This meta tag is nolonger supported by most search engines."),
    'class' => 'BackdropTextMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
  );

  $info['tags']['news_keywords'] = array(
    'label' => t('Google News Keywords'),
    'description' => t('A comma-separated list of keywords about the page. This meta tag is used as an indicator in <a href="@google_news">Google News</a>.', array('@google_news' => 'http://support.google.com/news/publisher/bin/answer.py?hl=en&answer=68297')),
    'class' => 'BackdropTextMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
  );

  $info['tags']['standout'] = array(
    'label' => t('Google Standout'),
    'description' => t("Highlight standout journalism on the web, especially for breaking news; used as an indicator in <a href=\"@google_news\">Google News</a>. Warning: Don't abuse it, to be used a maximum of 7 times per calendar week!", array('@google_news' => 'https://support.google.com/news/publisher/answer/191283?hl=en&ref_topic=2484650')),
    'class' => 'BackdropTextMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
  );

  $info['tags']['rating'] = array(
    'label' => t('Content rating'),
    'description' => t('Used to indicate the intended audience for the content.'),
    'class' => 'BackdropTextMetaTag',
    'group' => 'advanced',
    'select_or_other' => TRUE,
    'form' => array(
      '#type' => 'select',
      '#options' => array(
        'general' => t('General'),
        'mature' => t("Mature"),
        'restricted' => t("Restricted"),
        '14 years' => t("14 years or Older"),
        'safe for kids' => t("Safe for kids"),
      ),
      '#empty_option' => t('- None -'),
    ),
    'weight' => ++$weight,
  );

  $info['tags']['referrer'] = array(
    'label' => t('Referrer policy'),
    'description' => t('Indicate to search engines and other page scrapers whether or not links should be followed. See <a href="http://w3c.github.io/webappsec/specs/referrer-policy/">the W3C specifications</a> for further details.'),
    'class' => 'BackdropTextMetaTag',
    'group' => 'advanced',
    'select_or_other' => TRUE,
    'form' => array(
      '#type' => 'select',
      '#options' => array(
        'no-referrer' => t('No Referrer'),
        'origin' => t('Origin'),
        'no-referrer-when-downgrade' => t('No Referrer When Downgrade'),
        'origin-when-cross-origin' => t('Origin When Cross-Origin'),
        'unsafe-url' => t('Unsafe URL'),
      ),
      '#empty_option' => t('- None -'),
    ),
    'weight' => ++$weight,
  );

  $info['tags']['generator'] = array(
    'label' => t('Generator'),
    'description' => t("Describes the name and version number of the software or publishing tool used to create the page."),
    'class' => 'BackdropTextMetaTag',
    'header' => 'X-Generator',
    'context' => array('global'),
    'group' => 'advanced',
    'weight' => ++$weight,
  );

  $info['tags']['rights'] = array(
    'label' => t('Rights'),
    'description' => t("Details about intellectual property, such as copyright or trademarks; does not automatically protect the site's content or intellectual property."),
    'class' => 'BackdropTextMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
    'replaces' => array(
      'copyright',
    ),
  );

  $info['tags']['image_src'] = array(
    'label' => t('Image'),
    'description' => t("An image associated with this page, for use as a thumbnail in social networks and other services."),
    'class' => 'BackdropLinkMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
    'image' => TRUE,
    'devel_generate' => array(
      'type' => 'image',
    ),
  );

  $info['tags']['canonical'] = array(
    'label' => t('Canonical URL'),
    'description' => t("Preferred page location or URL to help eliminate duplicate content for search engines."),
    'class' => 'BackdropLinkMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
    'devel_generate' => array(
      'type' => 'canonical',
    ),
  );

  $info['tags']['shortlink'] = array(
    'label' => t('Shortlink URL'),
    'description' => t('A brief URL, often created by a URL shortening service.'),
    'class' => 'BackdropLinkMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
    'replaces' => array(
      'shorturl',
    ),
    'devel_generate' => array(
      'type' => 'shortlink',
    ),
  );

  $info['tags']['original-source'] = array(
    'label' => t('Original Source'),
    'description' => '',
    'class' => 'BackdropTextMetaTag',
    'group' => 'advanced',
    'description' => t("Used to indicate the URL that broke the story, and can link to either an internal URL or an external source. If the full URL is not known it is acceptable to use a partial URL or just the domain name."),
    'weight' => ++$weight,
    'devel_generate' => array(
      'type' => 'url',
    ),
  );

  $info['tags']['prev'] = array(
    'label' => t('Previous page URL'),
    'description' => t('Used for paginated content. Meet Google recommendations to <a href="@google_pagination">indicate paginated content</a> by providing URL with rel="prev" link.', array('@google_pagination' => 'https://support.google.com/webmasters/answer/1663744')),
    'class' => 'BackdropLinkMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
    'devel_generate' => array(
      'type' => 'url',
    ),
  );

  $info['tags']['next'] = array(
    'label' => t('Next page URL'),
    'description' => t('Used for paginated content. Meet Google recommendations to <a href="@google_pagination">indicate paginated content</a> by providing URL with rel="next" link.', array('@google_pagination' => 'https://support.google.com/webmasters/answer/1663744')),
    'class' => 'BackdropLinkMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
    'devel_generate' => array(
      'type' => 'url',
    ),
  );

  $info['tags']['content-language'] = array(
    'label' => t('Content language'),
    'description' => t("Used to define this page's language code. May be the two letter language code, e.g. \"de\" for German, or the two letter code with a dash and the two letter ISO country code, e.g. \"de-AT\" for German in Austria. Still used by Bing."),
    'class' => 'BackdropTextMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
    'is_language' => TRUE,
    'element' => array(
      '#theme' => 'metatag_http_equiv',
    ),
  );

  $info['tags']['geo.position'] = array(
    'label' => t('Geo position'),
    'description' => t('Geo-spatial information in "latitude;longitude" format, e.g. "50.167958;-97.133185"; <a href="http://en.wikipedia.org/wiki/Geotagging#HTML_pages">see Wikipedia for details</a>.'),
    'class' => 'BackdropTextMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
  );

  $info['tags']['geo.placename'] = array(
    'label' => t('Geo place name'),
    'description' => t("A location's formal name."),
    'class' => 'BackdropTextMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
  );

  $info['tags']['geo.region'] = array(
    'label' => t('Geo region'),
    'description' => t("A location's two-letter international country code, with an optional two-letter region, e.g. \"US-NH\" for New Hampshire in the USA."),
    'class' => 'BackdropTextMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
  );

  $info['tags']['icbm'] = array(
    'label' => t('ICBM'),
    'description' => t('Geo-spatial information in "latitude, longitude" format, e.g. "50.167958, -97.133185"; <a href="https://en.wikipedia.org/wiki/ICBM_address">see Wikipedia for details</a>.'),
    'class' => 'BackdropTextMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
  );

  $info['tags']['refresh'] = array(
    'label' => t('Refresh'),
    'description' => t('The number of seconds to wait before refreshing the page. May also force redirect to another page using the format "5; url=http://example.com/", which would be triggered after five seconds.'),
    'class' => 'BackdropTextMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
    'element' => array(
      '#theme' => 'metatag_http_equiv',
    ),
  );

  $info['tags']['revisit-after'] = array(
    'label' => t('Revisit After'),
    'description' => t('Tell search engines when to index the page again. Very few search engines support this tag, it is more useful to use an <a href="@xmlsitemap">XML Sitemap</a> file.', array('@xmlsitemap' => 'https://backdropcms.org/project/xmlsitemap')),
    'class' => 'BackdropDateIntervalMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
    'devel_generate' => array(
      'type' => 'date',
    ),
  );

  $info['tags']['pragma'] = array(
    'label' => t('Pragma'),
    'description' => t('Used to control whether a browser caches a specific page locally. Little used today. Should be used in conjunction with the Cache-Control meta tag.'),
    'class' => 'BackdropTextMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
    'element' => array(
      '#theme' => 'metatag_http_equiv',
    ),
  );

  $info['tags']['cache-control'] = array(
    'label' => t('Cache-Control'),
    'description' => t('Used to control whether a browser caches a specific page locally. Little used today. Should be used in conjunction with the Pragma meta tag.'),
    'class' => 'BackdropTextMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
    'element' => array(
      '#theme' => 'metatag_http_equiv',
    ),
  );

  $info['tags']['expires'] = array(
    'label' => t('Expires'),
    'description' => t("Control when the browser's internal cache of the current page should expire. The date must to be an <a href=\"@rfc\">RFC-1123</a>-compliant date string that is represented in Greenwich Mean Time (GMT), e.g. 'Thu, 01 Sep 2016 00:12:56 GMT'. Set to '0' to stop the page being cached entirely.", array('@rfc' => 'http://www.csgnetwork.com/timerfc1123calc.html')),
    'class' => 'BackdropTextMetaTag',
    'weight' => ++$weight,
    'group' => 'advanced',
    'devel_generate' => array(
      'type' => 'date',
    ),
  );

  // More advanced meta tags.
  $info['tags']['robots'] = array(
    'label' => t('Specific directions for robots:'),
    'class' => 'BackdropListMetaTag',
    'group' => 'advanced',
    'weight' => ++$weight,
    'form' => array(
      '#options' => array(
        'noindex' => t('Prevent search engines from indexing this page.'),
        'nofollow' => t('Prevent search engines from following links on this page.'),
        'noarchive' => t('Prevent cached copies of this page from appearing in search results.'),
        'nosnippet' => t('Prevent descriptions from appearing in search results, and prevents page caching.'),
        'noodp' => t('Block the <a href="!opendirectory">Open Directory Project</a> description from appearing in search results.', array('!opendirectory' => 'http://www.dmoz.org/')),
        'noydir' => t('Prevent Yahoo! from listing this page in the <a href="@ydir">Yahoo! Directory</a>.', array('@ydir' => 'http://dir.yahoo.com/')),
        'noimageindex' => t('Prevent search engines from indexing images on this page.'),
        'notranslate' => t('Prevent search engines from offering to translate this page in search results.'),
        'index' => t('Allow search engines to index this page (assumed).'),
        'follow' => t('Allow search engines to follow links on this page (assumed).'),
      ),
    ),
  );

  return $info;
}
