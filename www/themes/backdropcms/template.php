<?php
/**
 * @file
 * Theme and preprocess functions for the backdropcms theme.
 */

/*******************************************************************************
 * Alter functions: modify renderable structures before used.
 ******************************************************************************/


/*******************************************************************************
 * Preprocess functions: prepare variables for templates.
 ******************************************************************************/

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

/**
 * Preare varibles for node.tpl.php.
 */
function backdropcms_preprocess_node(&$variables) {
  if ($variables['type'] == 'feature') {
    $variables['icon_class'] = '';
    $node = $variables['node'];
    if (property_exists($node, 'field_icon_class') && !empty($node->field_icon_class)) {
      $class = $node->field_icon_class[LANGUAGE_NONE][0]['safe_value'];
      $variables['icon_class'] = $class;
    }
  }
}

/******************************************************************************
 * Theme function overrides
 ******************************************************************************/

/**
 * Overrides theme_menu_link().
 */
function backdropcms_menu_link(array $variables) {
  $element = $variables['element'];
  $sub_menu = '';

  if ($element['#below']) {
    $sub_menu = backdrop_render($element['#below']);
  }

  $menu_name = isset($element['#original_link']['menu_name']) ? $element['#original_link']['menu_name'] : NULL;
  if ($menu_name === 'main-menu' || $menu_name === 'menu-handbook') {
    // If this is the handbook link and we're on a book page, set an active class.
    if ($element['#href'] === 'node/1') {
      $node = menu_get_object();
      if (isset($node) && isset($node->type) && $node->type === 'book') {
        $element['#attributes']['class'][] = 'active';
      }
    }
  }

  if ($menu_name === 'main-menu' && (
        $element['#href'] == 'https://forum.backdropcms.org' ||
        $element['#href'] == 'https://docs.backdropcms.org' ||
        $element['#href'] == 'https://events.backdropcms.org')) {
    $title = check_plain($element['#title']);
    $element['#title'] = $title . ' <i class="fa fa-external-link" aria-hidden="true"></i>';
    $element['#localized_options']['html'] = TRUE;
  }

  $output = l($element['#title'], $element['#href'], $element['#localized_options']);
  return '<li' . backdrop_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}

/**
 * Overrides theme_field__body__docs().
 */
function backdropcms_field__body__docs($variables) {

  // Add bug squad members on the bug squad page.
  if ($variables['element']['#object']->nid == '2306') {
    // Safety check for the project metrics module.
    if (module_exists('borg_project_metrics')) {
      $bug_squad = borg_project_metrics_teams('3489194');
      $members = array();
      foreach ($bug_squad as $key => $member) {
        $info  = '<img class="gh-avatar" src="' . $member['avatar_url'] . '" />';
        $info .= '<strong>' . $member['name'] . '</strong>';
        $members[] = $info;
      }

      $output  = backdrop_render($variables['element'][0]);
      $output .= '<p class="bug-squad-header"><strong>Bug Squad Members</strong></p>';
      $output .= '<div class="container">';
      $output .= '  <div class="row">';
      $output .=      theme('item_list', array('items' => $members, 'attributes' => array('class' => array('leadership', 'bug-squad'))));
      $output .= '  </div> <!-- /.row -->';
      $output .= '</div> <!-- /.container -->';

      return $output;
    }
  }

  // Add security team members on the security team page.
  if ($variables['element']['#object']->nid == '2566') {
    // Safety check for the project metrics module.
    if (module_exists('borg_project_metrics')) {
      $sec_team = borg_project_metrics_teams('1817637');
      $members = array();
      foreach ($sec_team as $key => $member) {
        $info  = '<img class="gh-avatar" src="' . $member['avatar_url'] . '" />';
        $info .= '<strong>' . $member['name'] . '</strong>';
        $members[] = $info;
      }

      $output  = backdrop_render($variables['element'][0]);
      $output .= '<p class="bug-squad-header"><strong>Security Team Members</strong></p>';
      $output .= '<div class="container">';
      $output .= '  <div class="row">';
      $output .=      theme('item_list', array('items' => $members, 'attributes' => array('class' => array('leadership', 'bug-squad'))));
      $output .= '  </div> <!-- /.row -->';
      $output .= '</div> <!-- /.container -->';

      return $output;
    }
  }
}
