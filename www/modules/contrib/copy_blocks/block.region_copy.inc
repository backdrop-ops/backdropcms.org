<?php

/**
 *  BlockRegionCopy extends Block
 *
 *  This class allows us to display the blocks from a chosen region.
 */
class BlockRegionCopy extends Block {
  /**
   *  Sets title text on draggable block panel in Layout builder.
   */
  function getAdminTitle() {
    if (isset($this->settings['source_region'])) {
      // Get the source layout and region.
      list($layout_name, $region) = explode(':', $this->settings['source_region']);
      $status = '';
      $region_title = $region;
      $layout_title = $layout_name;
      $layout = layout_load($layout_name);
      if ($layout) {
        $layout_title = $layout->title;
        $layout_info = layout_get_layout_template_info($layout->layout_template);
        if (isset($layout_info['regions'][$region])) {
          $region_title = $layout_info['regions'][$region];
        }
        else {
          $status = ' ' . t('(region not found)');
        }
      }
      else {
        $status = ' ' . t('(layout not found)');
      }

      return t('Copy of %region region from %layout @status', array(
        '%region' => $region_title,
        '%layout' => $layout_title,
        '@status' => $status,
      ));
    }
    return t('Region copy block');
  }

  /**
   *  Sets block subject on block view.
   */
  function getTitle() {
    // Region blocks shouldn't have titles.
    return '';
  }

  /**
   * Returns the rendered content of this block.
   *
   * @return string
   */
  function getContent() {
    // Get the source layout and region.
    list($layout_name, $region) = explode(':', $this->settings['source_region']);
    $layout = layout_load($layout_name);

    if (isset($layout->positions[$region])) {
      $blocks = array();
      foreach ($layout->positions[$region] as $uuid) {
        if (isset($layout->content[$uuid])) {
          $block = $layout->content[$uuid];
          $blocks[] = ($block->delta != 'main' && $block->status) ? $this->renderBlock($layout, $block) : '';
        }
      }
      $rendered_region = $this->renderRegion($layout, $region, $blocks);
      return $rendered_region;
    }
    return '';
  }

  /**
   * Render a block using its designated style.
   *
   * @param Layout $layout
   *   The layout object containing the blocks to be copied.
   * @param Block $block
   *   The block to be rendered.
   *
   * @return string
   *   The rendered block as HTML.
   */
  function renderBlock($layout, $block) {
    $output = '';
    // Assign contexts to block.
    $layout_contexts = $layout->getContexts();

    $has_contexts = TRUE;
    $required_contexts = $block->getRequiredContexts();
    if ($required_contexts) {
      $has_contexts = $layout->hasContexts($required_contexts);
      if ($has_contexts) {
        $block->setContexts($layout_contexts);
      }
    }
    elseif ($block->conditions) {
      $block->setContexts($layout_contexts);
    }

    $block->prepare();
    if ($has_contexts && $block->checkAccess() && $content = $block->getContent()) {
      $style = layout_get_style_info($block->style->plugin);

      // Ensure block content is a renderable array for contextual links.
      if (is_string($content)) {
        $content = array(
          '#markup' => $content,
        );
      }
      // Add the contextual links.
      if (!($block->module === 'system' && $block->delta === 'main')) {
        $content['#contextual_links']['layout'] = array('admin/structure/layouts/manage/' . $layout->name . '/modal', array($block->uuid));
      }

      $title = '';
      if ($block->settings['title_display'] !== LAYOUT_TITLE_NONE) {
        $title = $block->getTitle();
      }

      // Assemble a structured content array.
      $content = (object) array(
        'title' => $title,
        'content' => $content,
      );

      if (isset($style) && isset($style['block theme'])) {
        $output = theme($style['block theme'] . '__' . $block->module . '__' . strtr($block->delta, '-', '_'), array('content' => $content, 'layout' => $layout, 'block' => $block, 'style' => $style, 'settings' => $block->style->settings));
      }
      else {
        // Fallback.
        $output = theme('block', array('content' => $content, 'layout' => $layout, 'block' => $block));
      }
    }
    return $output;
  }

  /**
   * Render the layout region to be copied.
   *
   * @param Layout $layout
   *   The layout object containing the blocks to be copied.
   * @param string $region_id
   *   The ID of the layout region being rendered.
   * @param array $blocks
   *   An array of blocks that are assigned to the region that's being rendered.
   *
   * @return string
   *   The rendered, HTML string output of the passed-in region.
   */
  function renderRegion($layout, $region_id, $blocks) {
    $settings = $layout->settings;
    if (empty($settings)) {
      $style_settings = $settings['style_settings']['default'] ?? array();
    }
    else {
      $style_settings = $settings[$region_id] ?? array();
    }
    $tag = $style_settings['element'] ?? '';
    $classes = array();
    if (isset($style_settings['classes'])) {
      $classes = explode(' ', $style_settings['classes']);
    }

    return theme('layout_region_inner', array('blocks' => $blocks, 'tag' => $tag, 'classes' => $classes));
  }

  /**
   *  Builds the block's configuration form.
   */
  function form(&$form, &$form_state) {
    global $user;
    parent::form($form, $form_state);
    $form['title_display']['#access'] = FALSE;

    $regions = array();
    $all_layouts = layout_load_all();

    $region = '';
    if (!empty($form_state['values']['source_layout'])) {
      // Form is currently in use, get most recent values.
      list($source_layout_name, $region) = explode(':', $form_state['values']['source_region'] ?? '');
      $source_layout_name = $form_state['values']['source_layout'];
    }
    elseif (!empty($this->settings['source_region'])) {
      // Use saved values.
      list($source_layout_name, $region) = explode(':', $this->settings['source_region']);
    }
    if (empty($source_layout_name)) {
      $source_layout_name = tempstore_get('copy_blocks', 'source_layout_' . $user->uid);
      if (empty($source_layout_name)) {
        $source_layout_name = 'default';
      }
    }
    $layout_options = array();
    $usage = layout_load_all();
    foreach ($usage as $layout_name => $layout) {
      $layout_options[$layout_name] = $layout->title;
    }
    if (in_array($source_layout_name, array_keys($layout_options))) {
      $form_state['source_layout'] = $source_layout_name;
      if (isset($all_layouts[$source_layout_name]->positions[$region])) {
        $region = $source_layout_name . ':' . $region;
      }
      else {
        $dest_region = $form['region']['#default_value'] ?? '';
        if (isset($all_layouts[$source_layout_name]->positions[$dest_region])) {
          $region = $source_layout_name . ':' . $dest_region;
        }
        else {
          $region = '';
        }
      }
    }
    $form_state['input']['source_region'] = $region;
    $form_state['values']['source_region'] = $region;

    $form['source_layout'] = array(
      '#type' => 'select',
      '#title' => t('Copy region from this layout'),
      '#default_value' => $form_state['source_layout'] ?? '',
      '#options' => $layout_options,
      '#ajax' => array(
        'wrapper' => 'layout_blocks_wrapper',
        'callback' => 'copy_blocks_ajax_select',
      ),
    );

    $form['layout_blocks_wrapper'] = array(
      '#type' => 'container',
      '#id' => 'layout_blocks_wrapper',
    );

    $form['layout_blocks_wrapper']['source_region'] = array(
      '#type' => 'radios',
      '#parents' => array('source_region'),
      '#title' => t('Copy this region'),
      '#options' => $this->layout_available_regions($form_state),
      '#default_value' => $region,
    );

    $form['plain_region'] = array(
      '#type' => 'checkbox',
      '#title' => t('Exclude outer HTML wrapper'),
      '#default_value' => $this->settings['plain_region'] ?? FALSE,
      '#description' => t('If checked, only the source blocks will be rendered, without wrapper elements.'),
    );
  }


  /**
   * Returns an array of available regions for the selected layout.
   */
  function layout_available_regions($form_state) {
    $regions = array();
    if (isset($form_state['values']['source_layout'])) {
      $form_state['source_layout'] = $form_state['values']['source_layout'];
    }

    $layout = layout_load($form_state['source_layout']);
    $template_info = layout_get_layout_template_info($layout->layout_template);

    foreach ($layout->positions as $region => $position) {
      if ($region != 'title') {
        $region_name = $template_info['regions'][$region];
        // Remove the the word "layout" from the end of layout names.
        $layout_title = str_ireplace(' layout', '', $layout->title);
        $regions[$form_state['source_layout'] . ':' . $region] = $region_name;
      }
    }

    return $regions;
  }

  /**
   * Validation handler to prevent blocks from copying themselves.
   */
  function formValidate($form, &$form_state) {
    parent::formValidate($form, $form_state);

    if ($form_state['values']['source_region'] == $form_state['layout']->name . ':' . $form_state['values']['region']) {
      form_set_error('source_region', t('Block cannot copy itself.'));
    }

  }

  /**
   * Submit handler to save the form settings.
   */
  function formSubmit($form, &$form_state) {
    parent::formSubmit($form, $form_state);
    global $user;
    $expire = REQUEST_TIME + 3600 * 24 * 30; // Set expiration time for ~ 1 month.
    $this->settings['source_region'] = $form_state['values']['source_region'];
    $this->settings['plain_region'] = $form_state['values']['plain_region'];
    tempstore_set('copy_blocks', 'source_layout_' . $user->uid, $form_state['values']['source_layout'], $expire);
  }
}
