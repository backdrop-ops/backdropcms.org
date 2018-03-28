<?php
/**
 * @file
 * Theme functions for the YouTube field module.
 */

/**
 * Theme function for videos.
 */
function theme_youtube_video($variables) {
  $id = $variables['video_id'];

  // Get field display settings.
  $size = $variables['size'];
  $width = array_key_exists('width', $variables) ? $variables['width'] : NULL;
  $height = array_key_exists('height', $variables) ? $variables['height'] : NULL;
  $autoplay = array_key_exists('autoplay', $variables) ? $variables['autoplay'] : FALSE;
  $loop = array_key_exists('loop', $variables) ? $variables['loop'] : FALSE;
  $showinfo = array_key_exists('showinfo', $variables) ? $variables['showinfo'] : FALSE;
  $controls = array_key_exists('controls', $variables) ? $variables['controls'] : FALSE;
  $autohide = array_key_exists('autohide', $variables) ? $variables['autohide'] : FALSE;
  $iv_load_policy = array_key_exists('iv_load_policy', $variables) ? $variables['iv_load_policy'] : FALSE;

  // Get YouTube settings.
  $suggest = config_get('youtube.settings', 'youtube_suggest');
  $privacy = config_get('youtube.settings', 'youtube_privacy');
  $modestbranding = config_get('youtube.settings', 'youtube_modestbranding');
  $theme = config_get('youtube.settings', 'youtube_theme');
  $color = config_get('youtube.settings', 'youtube_color');
  $enablejsapi = config_get('youtube.settings', 'youtube_enablejsapi');
  $player_class = config_get('youtube.settings', 'youtube_player_class');
  $wmode = config_get('youtube.settings', 'youtube_wmode');
  $privacy = config_get('youtube.settings', 'youtube_privacy');
  $dimensions = youtube_get_dimensions($size, $width, $height);

  // Query string changes based on setings.
  $query = array();
  if (!$suggest) {
    $query['rel'] = '0';
  }
  if ($modestbranding) {
    $query['modestbranding'] = '1';
  }
  if ($theme) {
    $query['theme'] = 'light';
  }
  if ($color) {
    $query['color'] = 'white';
  }
  if ($enablejsapi) {
    global $base_url;
    $query['enablejsapi'] = '1';
    $query['origin'] = $base_url;
  }
  if ($wmode) {
    $query['wmode'] = 'opaque';
  }
  if ($autoplay) {
    $query['autoplay'] = '1';
  }
  if ($loop) {
    $query['loop'] = '1';
    $query['playlist'] = $id;
  }
  if ($showinfo) {
    $query['showinfo'] = '0';
  }
  if ($controls) {
    $query['controls'] = '0';
  }
  if ($autohide) {
    $query['autohide'] = '1';
  }
  if ($iv_load_policy) {
    $query['iv_load_policy'] = '3';
  }

  // Domain changes based on settings.
  $domain = ($privacy) ? 'youtube-nocookie.com' : 'youtube.com';

  $path = 'https://www.' . $domain . '/embed/' . $id;
  $src = url($path, array('query' => $query));

  $player_title = t('Embedded video');
  if (!empty($variables['entity_title'])) {
    $player_title .= t(' for @entity_title', array(
      '@entity_title' => $variables['entity_title'],
    ));
  }

  $output = '<iframe id="' . backdrop_html_id($player_class) . '" class="' . $player_class . '"
    width="' . $dimensions['width'] . '" height="' . $dimensions['height'] . '"
    src="' . $src . '" title="' . $player_title . '" frameborder="0"
    allowfullscreen></iframe>';

  if ($size == 'responsive') {
    $output = '<div class="youtube-container--responsive">' . $output . '</div>';
  }

  return $output;
}


/**
 * Theme function for thumbnails.
 */
function theme_youtube_thumbnail($variables) {
  $id = $variables['video_id'];
  $style = $variables['image_style'];

  // Get YouTube settings - TODO is this needed?
  $size = variable_get('youtube_size', '420x315');
  $dimensions = youtube_get_dimensions($size);

  $files = config_get('system.core', 'file_public_path');
  $youtube = config_get('youtube.settings', 'youtube_thumb_dir');
  $dest = $files . '/' . $youtube . '/' . $id . '.png';

  // Check to see if a thumbnail exists locally.
  if (!file_exists($dest)) {
    // Retrieve the image from YouTube.
    if (!youtube_get_remote_image($id)) {
      // Use the remote source if local copy fails.
      $src = youtube_build_remote_image_path($id);
      return theme('image', array('uri' => $src));
    }
  }

  $alt = t('Embedded thumbnail');
  if (!empty($variables['entity_title'])) {
    $alt .= t(' for @entity_title', array(
      '@entity_title' => $variables['entity_title'],
    ));
  }

  if ($style) {
    $uri = 'public://' . $youtube . '/' . $id . '.png';
    $image = theme('image_style', array(
      'style_name' => $style,
      'uri' => $uri,
      'alt' => $alt,
    ));
  }
  else {
    $uri = $files . '/' . $youtube . '/' . $id . '.png';
    $image = theme('image', array('uri' => $uri, 'alt' => $alt));
  }

  // Check if an url path is provided
  if ($variables['image_link'] != NULL) {
    $url_path = $variables['image_link']['path'];
    $options = $variables['image_link']['options'];
    $image = l($image, $url_path, $options);
  }

  return $image;
}
