<?php
/**
 * @file
 * Theme and preprocess functions for views output.
 *
 * @todo move this file up a direcotry when the follwing issie is resolved:
 *   https://github.com/backdrop/backdrop-issues/issues/1468
 */

/**
 * Prepares vairables for Views JSON "Simple" format templates.
 *
 * - $view: The view in use.
 * - $rows: Array of row objects as rendered by _views_json_render_fields
 * - $attachment: Not used currently
 * - $options: The options for the style passed in from the UI.
 *
 * @see views_json.views.inc
 * @see views-json-style-tpl.php
 * @see views-views-json-style-simple.php
 */
function template_preprocess_views_views_json_style_simple(&$variables) {
  $view = $variables["view"];
  $rows = $variables["rows"];
  $options = $variables["options"];
  $base = $view->base_table;
  $root_object = $options["root_object"];
  $top_child_object = $options["top_child_object"];
  $plaintext_output = $options["plaintext_output"];
  $objects = array();

  // Create bitmask for json_encode.
  $option_defs = $variables['view']->style_plugin->option_definition();
  $bitmasks = $option_defs['encoding']['contains'];
  $bitmask = NULL;
  foreach ($bitmasks as $mask_key => $_bitmask) {
    if (isset($options[$mask_key]) && $options[$mask_key] && !is_array($options[$mask_key])) {
      $bitmask = $bitmask | constant($_bitmask['bitmask']);
    }
  }
  $variables['bitmask'] = $bitmask;

  foreach ($rows as $row) {

    $object = array();
    /* Convert the $rows into a hierachial key=>value array */
    foreach ($row as $field) {
      if ($options["field_output"] == "normal") {
        if ($field->label) {
          $label = $plaintext_output ? strip_tags($field->label) : $field->label;
        }
        else {
          $label = $plaintext_output ? strip_tags($field->id) : $field->id;
        }
        if (!$field->is_multiple) {
          if (is_array($field->content)) {
            $content = array();
            foreach ($field->content as $key => $value) {
              $safe = $plaintext_output ? strip_tags(html_entity_decode($value, ENT_QUOTES)) : $value;
              $safe = mb_check_encoding($safe, 'UTF-8') ? $safe : utf8_encode($safe);
              $content[$key] = $safe;
            }
          }
          else {
            $content = $plaintext_output ? strip_tags(html_entity_decode($field->content, ENT_QUOTES)) : $field->content;
            $content = mb_check_encoding($content, 'UTF-8') ? $content : utf8_encode($content);
          }
        }
        else {
          $content = array();
          foreach ($field->content as $n => $oc) {
            if (is_array($oc)) {
              foreach ($oc as $key => $value) {
                $content[$n][$key] = ($plaintext_output ? strip_tags($value) : $value);
              }
            }
            else {
              $content[$n] = ($plaintext_output ? strip_tags($oc) : $oc);
            }
          }
        }
      }
      elseif ($options["field_output"] == "raw") {
        $label = $plaintext_output ? strip_tags($field->id) : $field->id;
        if (!$field->is_multiple) {
          $content = $plaintext_output ? strip_tags($field->raw) : $field->raw;
        }
        else {
          $content = array();
          foreach ($field->raw as $n => $oc) {
            if (is_array($oc)) {
              foreach ($oc as $key => $value) {
                $content[$n][$key] = ($plaintext_output ? strip_tags($value) : $value);
              }
            }
            else {
              $content[$n] = ($plaintext_output ? strip_tags($oc) : $oc);
            }
          }
        }
      }

      // check if user wants nested arrays
      if (strlen($top_child_object) != 0) {
        $object[$top_child_object][$label] = $content;
      }
      else {
        $object[$label] = $content;
      }
    }
    $objects[] = $object;
  }

  // check if user wants nested arrays
  $variables["rows"] = strlen($root_object) != 0 ? array($root_object => $objects) : $objects;

  // For pagination, output the total number of pages, the current page, the
  // total result count, and the limit per page.
  global $pager_total, $pager_page_array, $pager_total_items, $pager_limits;
  if (isset($pager_total[0])) {
    $pager_object = array(
      'pages' => $pager_total[0],
      'page' => $pager_page_array[0],
      'count' => intval($pager_total_items[0]),
      'limit' => intval($pager_limits[0]),
    );
    $variables["rows"]["pager"] = $pager_object;
  }
}

/**
 * Prepares variables for Views JSON "single simple object" format templates.
 *
 * Sets only one row as view rows as object with properties.
 *
 * @see views_json.views.inc
 * @see views-views-json-style-simple-object.tpl.php
 */
function template_preprocess_views_views_json_style_simple_object(&$variables) {
  $rows = &$variables['rows'];
  $options = $variables["options"];
  $root_object = $options["root_object"];
  $top_child_object = $options["top_child_object"];
  $size = count($rows);

  if ($size < 1) {
    $rows = new stdClass();
    return;
  }

  // We need just only one element.
  $rows = $size < 1 ? $rows : array_slice($rows, 0, 1);

  template_preprocess_views_views_json_style_simple($variables);

  $row = empty($root_object) ? $rows[0] : $rows[$root_object][0];

  if (array_key_exists($top_child_object, $row)) {
    $row = $row[$top_child_object];
  }

  $rows = (object) $row;
}

/**
 * Prepares variables for JSON Simile/Exhibit templates.
 *
 * @see views_json.views.inc
 * @see views-views-json-style-exhibit.tpl.php
 */
function template_preprocess_views_views_json_style_exhibit(&$variables) {
  $view = $variables["view"];
  $rows = $variables["rows"];
  $options = $variables["options"];
  $base = $view->base_table;
  $root_object = "Items";
  $top_child_object = $options["top_child_object"];
  $plaintext_output = $options["plaintext_output"];
  $objects = array();
  foreach ($rows as $row) {
    $object = array($top_child_object => array());
    /* Convert the $rows into a hierachial key=>value array */
    foreach ($row as $field) {
      if ($options["field_output"] == "normal") {
        if ($field->label) {
          $label = $plaintext_output ? strip_tags($field->label) : $field->label;
        }
        else {
          $label = $plaintext_output ? strip_tags($field->id) : $field->id;
        }
        if (!$field->is_multiple) {
          $content = $plaintext_output ? strip_tags(html_entity_decode($field->content, ENT_QUOTES)) : $field->content;
          $content = mb_check_encoding($content, 'UTF-8') ? $content : utf8_encode($content);
        }
        else {
          $content = array();
          foreach ($field->content as $n => $oc) {
            $content[$n] = ($plaintext_output ? strip_tags($oc) : $oc);
          }
        }
      }
      elseif ($options["field_output"] == "raw") {
        $label = $plaintext_output ? strip_tags($field->id) : $field->id;
        if (!$field->is_multiple) {
          $content = $plaintext_output ? strip_tags($field->raw) : $field->raw;
        }
        else {
          $content = array();
          foreach ($field->raw as $n => $oc) {
            $content[$n] = $plaintext_output ? strip_tags($oc) : $oc;
          }
        }
      }

      // check if user wants nested arrays
      if (strlen($top_child_object) != 0) {
        $object[$top_child_object][$label] = $content;
      }
      else {
        $object[$label] = $content;
      }
    }
    if (!array_key_exists("label", $object)) {
      $object["label"] = "Item";
    }
    if (!array_key_exists("type", $object)) {
      $object["type"] = $top_child_object;
    }
    $objects[] = $object;
  }

  // check if user wants nested arrays
  $variables["rows"] = strlen($root_object) != 0 ? array($root_object => $objects) : $objects;
}

/**
 * Prepares variables for...
 *
 * @see
 */
function template_preprocess_views_views_json_style_jqgrid(&$variables) {
  $view = $variables["view"];
  $rows = $variables["rows"];
  $options = $variables["options"];
  $base = $view->base_table;
  $root_object = $options["root_object"];
  $top_child_object = $options["top_child_object"];
  $plaintext_output = $options["plaintext_output"];
  $objects = array();

  foreach ($rows as $row) {
    $object = array();
    foreach ($row as $field) {
      if ($options["field_output"] == "normal") {
        if ($field->label) {
          $label = $plaintext_output ? strip_tags($field->label) : $field->label;
        }
        else {
          $label = $plaintext_output ? strip_tags($field->id) : $field->id;
        }

        if (!$field->is_multiple) {
          $content = $plaintext_output ? strip_tags(html_entity_decode($field->content, ENT_QUOTES)) : $field->content;
          $content = mb_check_encoding($content, 'UTF-8') ? $content : utf8_encode($content);
        }
        else {
          $content = array();
          foreach ($field->content as $n => $oc) {
            $content[$n] = ($plaintext_output ? strip_tags($oc) : $oc);
          }
        }
      }
      elseif ($options["field_output"] == "raw") {
        $label = $plaintext_output ? strip_tags($field->id) : $field->id;
        if (!$field->is_multiple) {
          $content = $plaintext_output ? strip_tags($field->raw) : $field->raw;
        }
        else {
          $content = array();
          foreach ($field->raw as $n => $oc) {
            $content[$n] = $plaintext_output ? strip_tags($oc) : $oc;
          }
        }
      }
      $object[$label] = $content;
    }
    $objects[] = $object;
  }

  if (isset($view->query->pager)) {
    global $pager_total, $pager_total_items;

    $pager_id = $view->query->pager->options['id'];
    $very_top['page'] = $view->query->pager->current_page + 1;
    $very_top['records'] = $pager_total_items[$pager_id];
    $very_top['total'] = $pager_total[$pager_id];
    $very_top['rows'] = $objects;
  }

  $variables["rows"] = $very_top;//array($root_object => $objects);
}

/**
 * Prepares variables for...
 *
 * @see
 */
function template_preprocess_views_views_json_style_autocomplete(&$variables) {
  // Use simple theme suggestion.
  $variables['theme_hook_suggestion'] = 'views_views_json_style_simple';

  $view = $variables["view"];
  $rows = $variables["rows"];
  $options = $variables["options"];
  $plaintext_output = $options["plaintext_output"];

  // Create bitmask for json_encode.
  $option_defs = $variables['view']->style_plugin->option_definition();
  $bitmasks = $option_defs['encoding']['contains'];
  $bitmask = NULL;
  foreach ($bitmasks as $mask_key => $_bitmask) {
    $mask_options = &$options[$mask_key];
    if (isset($mask_options) && $mask_options && !is_array($mask_options)) {
      $bitmask = $bitmask | constant($_bitmask['bitmask']);
    }
  }
  $variables['bitmask'] = $bitmask;

  $autocomplete = array();
  foreach ($rows as $row) {
    // First field is the key and second is the value.
    $k = 0;
    foreach ($row as $field) {
      if ($options["field_output"] == "normal") {
        if (!$field->is_multiple) {
          if ($plaintext_output) {
            $decoded_content = html_entity_decode($field->content, ENT_QUOTES);
            $content = strip_tags($decoded_content);
          }
          else {
            $content = $field->content;
          }
          if (!mb_check_encoding($content, 'UTF-8')) {
            $content = utf8_encode($content);
          }
        }
        else {
          $content = array();
          foreach ($field->content as $n => $oc) {
            if ($plaintext_output) {
              $content[$n] = strip_tags($oc);
            }
            else {
              $content[$n] = $oc;
            }
          }
        }
      }
      elseif ($options["field_output"] == "raw") {
        if (!$field->is_multiple) {
          $content = $plaintext_output ? strip_tags($field->raw) : $field->raw;
        }
        else {
          $content = array();
          foreach ($field->raw as $n => $oc) {
            $content[$n] = $plaintext_output ? strip_tags($oc) : $oc;
          }
        }
      }

      // First field is the key.
      if ($k++ == 0) {
        $label = $content;
      }
      else {
        // Second field is value, so now we can stop.
        break;
      }
    }

    $autocomplete[$label] = $content;
  }

  $variables["rows"] = $autocomplete;
}
