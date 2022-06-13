<?php

/**
 *  BlockCopy extends Block
 *
 *  This class allows us to create blocks which are copies of other blocks.
 */
class BlockCopy extends Block {
  /**
   * {@inheritdoc}
   */
  function __construct($plugin_name, array $data) {
    parent::__construct($plugin_name, $data);

  }

  /**
   *  Sets title text on draggable block panel in Layout builder.
   */
  function getAdminTitle() {
    if (isset($this->settings['source_block'])) {
      list($module, $delta, $layout, $uuid) = explode(':', $this->settings['source_block']);
      $parent_layout = layout_load($layout);
      $parent_block = $parent_layout->content[$uuid];
      $parent_block_info = $this->getParentBlockInfo($parent_block);
      return t('Copy of %block from %parent_layout', array('%block' => $parent_block_info['info'], '%parent_layout' => $parent_layout->title));
    }
    return t('Copy block');
  }

  /**
   *  Sets block subject on block view.
   */
  function getTitle() {
    $parent_block = $this->getParentBlock();
    return isset($parent_block->settings['title']) ? check_plain($parent_block->settings['title']) : '';
  }

  /**
   * Returns the rendered content of this block.
   *
   * @return string
   */
  function getContent() {
    if (!empty($this->settings['source_block'])) {
      $parent_block = $this->getParentBlock();
      return $parent_block->getContent();
    }
  }

  /**
   *  Builds the block's configuration form.
   */
  function form(&$form, &$form_state) {
    backdrop_add_css(backdrop_get_path('module', 'copy_blocks') . '/css/copy-blocks.css');
    parent::form($form, $form_state);
    // Hide the title display.
    $form['title_display']['#access'] = FALSE;

    $all_block_info = layout_get_block_info();

    if (!isset($this->settings['source_block'])) {
      $form_state['source_layout'] = 'default';
      $layout_options = array();
      $usage = layout_load_all();
      foreach ($usage as $layout_name => $layout) {
        $layout_options[$layout_name] = $layout->title;
      }


      $form['source_layout'] = array(
        '#type' => 'select',
        '#title' => t('Copy block from this layout'),
        '#default_value' => $form_state['source_layout'],
        '#options' => $layout_options,
        '#ajax' => array(
          'wrapper' => 'layout_blocks_wrapper',
          'callback' => 'copy_blocks_ajax_select'
        ),
      );
      $form['layout_blocks_wrapper'] = array(
        '#type' => 'container',
        '#id' => 'layout_blocks_wrapper',
      );
      $form['layout_blocks_wrapper']['source_block'] = array(
        '#type' => 'radios',
        '#title' => t('Copy this block'),
        '#parents' => array('source_block'),
        '#default_value' => $form_state['source_block'],
        '#options' => $this->layout_available_blocks($form_state),
      );
    }
    else {
      list($module, $delta, $layout, $uuid) = explode(':', $this->settings['source_block']);
      $parent_layout = layout_load($layout);
      $block = $parent_layout->content[$uuid];
      $block_name = $all_block_info[$module][$delta]['info'];
      $edit_link = l(t('editing the parent block'), 'admin/structure/layouts/manage/' . $layout . '/configure-redirect/' . $uuid, array('fragment' => 'configure-block:' . $uuid));


      $form['copy_message'] = array(
        '#markup' => t('This is a copy of !block_name block. Settings may be modified by !edit_link. This will load the parent block layout and will open the parent block\'s configuration form', array('!block_name' => $block_name, '!edit_link' => $edit_link)),
      );
    }
  }

  /**
   * Submit handler to save the form settings.
   */
  function layout_available_blocks($form_state) {
    if (!isset($form_state['values']['source_layout'])) {
      $form_state['source_layout'] = 'default';
    }
    else {
      $form_state['source_layout'] = $form_state['values']['source_layout'];
    }
    $blocks = array();
    $all_block_info = layout_get_block_info();
    $layout = layout_load($form_state['source_layout']);
    $template_info = layout_get_layout_template_info($layout->layout_template);

    foreach ($layout->positions as $position => $uuids) {
      foreach ($uuids as $uuid) {
        $block = $layout->content[$uuid];
        $block->layout_title = $layout->title;
        $blocks[$block->module][$block->delta][$layout->name][$position][] = $block;
      }
    }
    foreach ($blocks as $module => $block) {
      foreach ($block as $block_delta => $layout_blocks) {
        foreach ($layout_blocks as $layout_name => $block_positions) {
          if (isset($all_block_info[$module])) {
            if ($layout_name == $form_state['source_layout']) {
              foreach ($block_positions as $position => $block_instances) {
                foreach ($block_instances as $key => $instance) {
                  $key_string = $key > 0 ? ': ' . t('instance !count', array('!count' => $instance + 1)) : '';
                  $position = $template_info['regions'][$position];
                  $block_title = $all_block_info[$module][$block_delta]['info'];
                  // Some block names have 'block' on the end.
                  $block_title = preg_replace('/ block$/', '', $block_title);

                  $instance_info = '<span class="copy-blocks-text">' . t(' block from ') . '</span>' . $position . '<span class="copy-blocks-text">' . t(' region') . '</span>' . $key_string;
                  $layout_available_blocks[$module . ':' . $block_delta . ':' . $layout_name . ':' . $instance->uuid] = '<strong>' . $block_title . '</strong>' . ' ' . $instance_info;
                }
              }
            }
          }
        }
      }
    }

    return $layout_available_blocks;
  }

  /**
   * Submit handler to save the form settings.
   */
  function formSubmit($form, &$form_state) {
    parent::formSubmit($form, $form_state);

    if (!isset($this->settings['source_block'])) {
      $this->settings['source_block'] = $form_state['values']['source_block'];
    }
  }

  /**
   * Get the block which is being copied.
   */
  function getParentBlock() {
    if (isset($this->settings['source_block'])) {
      list($module, $delta, $layout, $uuid) = explode(':', $this->settings['source_block']);
      $parent_layout = layout_load($layout);
      return $parent_layout->content[$uuid];
    }
    return NULL;
  }

  /**
   * Get block info for the block which is being copied.
   */
  function getParentBlockInfo($block) {
    $all_block_info = layout_get_block_info();
    return $all_block_info[$block->module][$block->delta];
  }
}