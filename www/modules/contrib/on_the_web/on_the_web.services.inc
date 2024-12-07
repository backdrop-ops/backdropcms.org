<?php
/**
 * @file
 * List of default servies for On The Web module.
 * See hook_on_the_web_get_services_alter() for how to add more.
 */

/**
 * Gets the available services.
 *
 * @return array
 *   List of services with icons.
 */
function on_the_web_get_services() {
  $services = array(
    /* Icons that are the FontAwesome "Brand" style. */
    'bluesky' => array(
      'name' => 'Bluesky',
      'fa-icon' => 'fa-bluesky',
      'fa-icon-square' => 'fa-bluesky-square',
      'v5style' => 'fab', // Brand
      'svg' => 'bluesky.svg',
      'svg-square' => 'bluesky-square.svg',
      'icon' => 'butterfly-fill',
    ),
    'delicious' => array(
      'name' => 'Delicious',
      'fa-icon' => 'fa-delicious',
      'v5style' => 'fab', // Brand
      'svg' => 'delicious.svg',
      'icon' => 'checkerboard-fill',
    ),
    'facebook' => array(
      'name' => 'Facebook',
      'fa-icon' => 'fa-facebook-f',
      'fa-icon-square' => 'fa-facebook-square',
      'v5style' => 'fab', // Brand
      'svg' => 'facebook-f.svg',
      'svg-square' => 'facebook-square.svg',
      'icon' => 'facebook-logo-fill',
    ),
    'flickr' => array(
      'name' => 'Flickr',
      'fa-icon' => 'fa-flickr',
      'v5style' => 'fab', // Brand
      'svg' => 'flickr.svg',
      'icon' => 'camera-fill',
    ),
    'github' => array(
      'name' => 'GitHub',
      'fa-icon' => 'fa-github',
      'fa-icon-square' => 'fa-github-square',
      'v5style' => 'fab', // Brand
      'svg' => 'github.svg',
      'svg-square' => 'github-square.svg',
      'icon' => 'github-logo-fill',
    ),
    'instagram' => array(
      'name' => 'Instagram',
      'fa-icon' => 'fa-instagram',
      'v5style' => 'fab', // Brand
      'svg' => 'instagram.svg',
      'svg-square' => 'instagram-square.svg',
      'icon' => 'instagram-logo-fill',
    ),
    'lastfm' => array(
      'name' => 'LastFM',
      'fa-icon' => 'fa-lastfm',
      'fa-icon-square' => 'fa-lastfm-square',
      'svg' => 'lastfm.svg',
      'svg-square' => 'lastfm-square.svg',
      'v5style' => 'fab', // Brand
      'icon' => 'lastfm-logo-fill',
    ),
    'linkedin' => array(
      'name' => 'LinkedIn',
      'fa-icon' => 'fa-linkedin',
      'fa-icon-square' => 'fa-linkedin-square',
      'v5style' => 'fab', // Brand
      'svg' => 'linkedin-in.svg',
      'svg-square' => 'linkedin-square.svg',
      'icon' => 'linkedin-logo-fill',
    ),
    'mastodon' => array(
      'name' => 'Mastodon',
      'fa-icon' => 'fa-mastodon',
      'fa-icon-square' => 'fa-mastodon-square',
      'v5style' => 'fab', // Brand
      'svg' => 'mastodon.svg',
      'icon' => 'mastodon-logo-fill',
    ),
    'pinterest' => array(
      'name' => 'Pinterest',
      'fa-icon' => 'fa-pinterest',
      'fa-icon-square' => 'fa-pinterest-square',
      'v5style' => 'fab', // Brand
      'svg' => 'pinterest.svg',
      'svg-square' => 'pinterest-square.svg',
      'icon' => 'pinterest-logo-fill',
    ),
    'soundcloud' => array(
      'name' => 'Soundcloud',
      'fa-icon' => 'fa-soundcloud',
      'v5style' => 'fab', // Brand
      'svg' => 'soundcloud.svg',
      'icon' => 'soundcloud-logo-fill',
    ),
    'tiktok' => array(
      'name' => 'TikTok',
      'fa-icon' => 'fa-tiktok',
      'v5style' => 'fab', // Brand
      'svg' => 'tiktok.svg',
      'icon' => 'tiktok-logo-fill',
    ),
    'twitter' => array(
      'name' => 'Twitter',
      'fa-icon' => 'fa-twitter',
      'fa-icon-square' => 'fa-twitter-square',
      'v5style' => 'fab', // Brand
      'svg' => 'twitter.svg',
      'svg-square' => 'twitter-square.svg',
      'icon' => 'twitter-logo-fill',
    ),
    'youtube' => array(
      'name' => 'YouTube',
      'fa-icon' => 'fa-youtube',
      'fa-icon-square' => 'fa-youtube-square',
      'v5style' => 'fab', // Brand
      'svg' => 'youtube.svg',
      'svg-square' => 'youtube-square.svg',
      'icon' => 'youtube-logo-fill',
    ),

    /* Icons that are the FontAwesome "Solid" style. */
    'envelope' => array(
      'name' => 'Newsletter (or Contact)',
      'fa-icon' => 'fa-envelope',
      'fa-icon-square' => 'fa-envelope-square',
      'v5style' => 'fas', // Solid
      'svg' => 'envelope.svg',
      'svg-square' => 'envelope-square.svg',
      'icon' => 'envelope-fill',
    ),
    'itunes' => array(
      'name' => 'Podcast',
      'fa-icon' => 'fa-podcast',
      'v5style' => 'fas', // Solid
      'svg' => 'podcast.svg',
      'icon' =>'microphone-fill',
    ),
    'rss' => array(
      'name' => 'RSS Feed',
      'fa-icon' => 'fa-rss',
      'fa-icon-square' => 'fa-rss-square',
      'v5style' => 'fas', // Solid
      'svg' => 'rss.svg',
      'svg-square' => 'rss-square.svg',
      'icon' => 'rss-fill',
    ),
  );

  backdrop_alter('on_the_web_get_services', $services);

  return $services;
}
