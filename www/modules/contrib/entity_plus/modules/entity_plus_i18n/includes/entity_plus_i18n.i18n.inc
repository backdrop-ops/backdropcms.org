<?php
/**
 * @file
 * Defines the controller for internationalization (i18n) integration.
 */

/**
 * Default controller handling i18n integration.
 *
 * Implements i18n string translation for all non-field properties marked as
 * 'translatable' and having the flag 'i18n string' set. This translation
 * approach fits in particular for translating configuration, i.e. exportable
 * entities.
 *
 * Requirements for the default controller:
 *  - The entity type providing module must be specified using the 'module' key
 *    in hook_entity_info().
 *  - An 'entity class' derived from the provided class 'Entity' must be used.
 *  - Properties must be declared as 'translatable' and the 'i18n string' flag
 *    must be set to TRUE using hook_entity_property_info().
 *  - i18n must be notified about changes manually by calling
 *    i18n_string_object_update(), i18n_string_object_remove() and
 *    i18n_string_update_context(). Ideally, this is done in a small integration
 *    module depending on the entity API and i18n_string. Look at the provided
 *    testing module "entity_test_i18n" for an example.
 *  - If the entity API admin UI is used, the "translate" tab will be
 *    automatically enabled and linked from the UI.
 *  - There are helpers for getting translated values which work regardless
 *    whether the i18n_string module is enabled, i.e. entity_i18n_string()
 *    and Entity::getTranslation().
 *
 *  Current limitations:
 *   - Translatable property values cannot be updated via the metadata wrapper,
 *     however reading works fine. See Entity::getTranslation().
 */
class EntityDefaultI18nStringController {

  protected $entityType, $entityInfo;

  /**
   * The i18n textgroup we are using.
   */
  protected $textgroup;

  public function __construct($type) {
    $this->entityType = $type;
    $this->entityInfo = entity_get_info($type);
    // By default we go with the module name as textgroup.
    $this->textgroup = $this->entityInfo['module'];
  }

  /**
   * Implements hook_i18n_string_info() via entity_i18n_string_info().
   */
  public function hook_string_info() {
    $list = system_list('module_enabled');
    $info = $list[$this->textgroup]->info;

    $groups[$this->textgroup] = array(
      'title' => $info['name'],
      'description' => !empty($info['description']) ? $info['description'] : NULL,
      'format' => FALSE,
      'list' => TRUE,
    );
    return $groups;
  }

  /**
   * Implements hook_i18n_object_info() via entity_i18n_object_info().
   *
   * Go with the same default values as the admin UI as far as possible.
   */
  public function hook_object_info() {
    $wildcard = $this->menuWildcard();
    $id_key = !empty($this->entityInfo['entity keys']['name']) ? $this->entityInfo['entity keys']['name'] : $this->entityInfo['entity keys']['id'];

    $info[$this->entityType] = array(
      // Generic object title.
      'title' => $this->entityInfo['label'],
      // The object key field.
      'key' => $id_key,
      // Placeholders for automatic paths.
      'placeholders' => array(
        $wildcard => $id_key,
      ),

      // Properties for string translation.
      'string translation' => array(
        // Text group that will handle this object's strings.
        'textgroup' => $this->textgroup,
        // Object type property for string translation.
        'type' => $this->entityType,
        // Translatable properties of these objects.
        'properties' => $this->translatableProperties(),
      ),
    );

    // Integrate the translate tab into the admin-UI if enabled.
    if ($base_path = $this->menuBasePath()) {
      $info[$this->entityType] += array(
        // To produce edit links automatically.
        'edit path' => $base_path . '/manage/' . $wildcard,
        // Auto-generate translate tab.
        'translate tab' => $base_path . '/manage/' . $wildcard . '/translate',
      );
      $info[$this->entityType]['string translation'] += array(
        // Path to translate strings to every language.
        'translate path' => $base_path . '/manage/' . $wildcard . '/translate/%i18n_language',
      );
    }
    return $info;
  }

  /**
   * Defines the menu base path used by self::hook_object_info().
   */
  protected function menuBasePath() {
    return !empty($this->entityInfo['admin ui']['path']) ? $this->entityInfo['admin ui']['path'] : FALSE;
  }

  /**
   * Defines the menu wildcard used by self::hook_object_info().
   */
  protected function menuWildcard() {
    return isset($this->entityInfo['admin ui']['menu wildcard']) ? $this->entityInfo['admin ui']['menu wildcard'] : '%entity_ui_object';
  }

  /**
   * Defines translatable properties used by self::hook_object_info().
   */
  protected function translatableProperties() {
    $list = array();
    foreach (entity_plus_get_all_property_info($this->entityType) as $name => $info) {
      if (!empty($info['translatable']) && !empty($info['i18n string'])) {
        $list[$name] = array(
          'title' => $info['label'],
        );
      }
    }
    return $list;
  }

  /**
   * Implements hook_i18n_string_objects() via entity_i18n_string_objects().
   */
  public function hook_string_objects() {
    return entity_load_multiple_by_name($this->entityType, FALSE);
  }

}
