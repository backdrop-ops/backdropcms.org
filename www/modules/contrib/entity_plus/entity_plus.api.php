<?php
/**
 * @file
 * Documents hook_entity_info() expansions by Entity Plus module.
 */

/**
 * This is a placeholder for describing further keys for hook_entity_info(),
 * which are introduced by Entity Plus for providing a new entity type with the
 * entity CRUD API. For that Entity Plus provides two controllers:
 *  - EntityPlusController: A regular CRUD controller.
 *  - EntityPlusControllerExportable: Extends the regular controller to
 *    additionally support exportable entities and/or entities making use of a
 *    name key.
 *
 * @return
 *   An array whose keys are entity type names and whose values identify
 *   properties of those types that the system needs to know about. 
 * 
 *   The following keys are defined by core's Entity module through hook_entity_info(): 
 * 
 *   - label: The human-readable name of the type.
 *   - entity class: A class that the controller will use for instantiating
 *     entities. Must extend the Entity abtract class or implement EntityInterface.
 *   - controller class: The name of the class that is used to load the objects.
 *     The class has to implement the EntityPlusControllerInterface interface.
 *     Entity Plus provides the class extensions EntityPlusController, used by 
 *     content entities, and EntityPlusControllerExportable, used by 
 *     configuration entities such as entities that define bundles of content entities.
 *   - base table: (used by EntityPlusController) The name of the
 *     entity type's base table.
 *   - static cache: (used by DefaultEntityController) FALSE to disable
 *     static caching of entities during a page request. Defaults to TRUE.
 *   - entity cache: (used by DefaultEntityController) Set to TRUE to enable
 *     persistent caching of fully loaded entities. This will be considered to
 *     be FALSE if there is not a cache table for the entity. Defaults to FALSE.
 *   - field cache: (used by Field API loading and saving of field data) FALSE
 *     to disable Field API's persistent cache of field data. Setting this to
 *     FALSE is recommended if a higher level persistent cache is available for
 *     the entity type. Defaults to TRUE.
 *   - load hook: The name of the hook which should be invoked by
 *     DefaultEntityController:attachLoad(), for example 'node_load'.
 *   - fieldable: Set to TRUE if you want your entity type to be fieldable.
 *   - translation: An associative array of modules registered as field
 *     translation handlers. Array keys are the module names, array values
 *     can be any data structure the module uses to provide field translation.
 *     Any empty value disallows the module to appear as a translation handler.
 *   - entity keys: An array describing additional information about the entity.
 *     This is used by both the EntityFieldQuery class and the Field API.
 *     EntityFieldQuery assumes at the very least that the id is always present.
 *     The Field API uses this array to extract the information it needs from
 *     the objects of the type. Elements:
 *     - id: The name of the property that contains the primary id of the
 *       entity. Every entity object passed to the Field API must have this
 *       property and its value must be numeric.
 *     - revision: The name of the property that contains the revision id of
 *       the entity. The Field API assumes that all revision ids are unique
 *       across all entities of a type. This entry can be omitted if the
 *       entities of this type are not versionable. Defaults to an empty string.
 *     - bundle: The name of the property that contains the bundle name for the
 *       entity. The bundle name defines which set of fields are attached to
 *       the entity (e.g. what nodes call "content type"). This entry can be
 *       omitted if this entity type exposes a single bundle (all entities have
 *       the same collection of fields). The name of this single bundle will be
 *       the same as the entity type.
 *   - bundle keys: An array describing how the Field API can extract the
 *     information it needs from the bundle objects for this type. This entry can
 *     be omitted if this type's bundles do not exist as standalone objects.
 *     Elements:
 *     - bundle: The name of the property that contains the name of the bundle
 *       object.
 *   - bundles: An array describing all bundles for this object type. Keys are
 *     bundles machine names, as found in the objects' 'bundle' property
 *     (defined in the 'entity keys' entry above). This entry can be omitted if
 *     this entity type exposes a single bundle (all entities have the same
 *     collection of fields). The name of this single bundle will be the same as
 *     the entity type. Defaults to an empty string. Elements:
 *     - label: The human-readable name of the bundle.
 *     - admin: An array of information that allows Field UI pages to attach
 *       themselves to the existing administration pages for the bundle.
 *       Elements:
 *       - path: the path of the bundle's main administration page, as defined
 *         in hook_menu(). If the path includes a placeholder for the bundle,
 *         the 'bundle argument', 'bundle helper' and 'real path' keys below
 *         are required.
 *       - bundle argument: The position of the placeholder in 'path', if any.
 *       - real path: The actual path (no placeholder) of the bundle's main
 *         administration page. This will be used to generate links.
 *       - access callback: As in hook_menu(). 'user_access' will be assumed if
 *         no value is provided.
 *       - access arguments: As in hook_menu().
 *   - view modes: An array describing the display modes for the entity type.
 *     Display modes let entities be displayed differently depending on the
 *     context. For instance, a node can be displayed differently on its own
 *     page ('full' mode), on the home page or taxonomy listings ('teaser'
 *     mode), or in an RSS feed ('rss' mode). Modules taking part in the display
 *     of the entity (notably the Field API) can adjust their behavior depending
 *     on the requested display mode. An additional 'default' display mode is
 *     available for all entity types. This display mode is not intended for
 *     actual entity display, but holds default display settings. For each
 *     available display mode, administrators can configure whether it should use
 *     its own set of field display settings, or just replicate the settings of
 *     the 'default' display mode, thus reducing the amount of display
 *     configurations to keep track of. Keys of the array are display mode
 *     names. Each display mode is described by an array with the following
 *     key/value pairs:
 *     - label: The human-readable name of the display mode.
 *     - custom settings: A boolean specifying whether the display mode should
 *       by default use its own custom field display settings. If FALSE,
 *       entities displayed in this display mode will reuse the 'default'
 *       display settings by default (e.g. right after the module exposing the
 *       display mode is enabled), but administrators can later use the Field UI
 *       to apply custom display settings specific to the display mode.
 * 
 *    The following keys are processed by Entity Plus:
 * 
 *   - bundle of: (optional) Entity types can be used as bundle definitions for
 *     other entity types. To enable this functionality, use the 'bundle of' key
 *     to indicate which entity type this entity serves as a bundle difnition for. But note
 *     that the other entity type will still need to declare entities of this
 *     type as bundles using the 'bundles' key, as described by the documentation of hook_entity_info().
 *     If the other entity type is fieldable, the entity API controller takes
 *     care of invoking the field API bundle attachers. Note that
 *     field_attach_delete_bundle() has to be invoked manually upon module
 *     uninstallation. 
 *   - exportable: (optional) Whether the entity is exportable. Defaults to FALSE.
 *     If enabled, a name key should be specified and db columns for the module
 *     and status key have to exist in the entity's base table. Also see 'entity keys' below.
 *     This option requires the EntityAPIControllerExportable to work.
 *   - entity keys: In addition to the sub-keys described above, Entity Plus processes the following
 *     elements:
 *       - name (optional): The key of the entity property containing the unique,
 *         machine readable name of the entity. If specified, this is used as
 *         identifier of the entity, while the usual 'id' key is still required and
 *         may be used when modules deal with entities generically, or to refer to
 *         the entity internally, i.e. in the database. For configuration entities that
 *         define bundles, this is typically 'name' => 'type'. 
 *         If a name key is given, the name is used as entity identifier by the
 *         Entity Plus module, metadata wrappers and entity-type specific hooks.
 *         However note that for consistency all generic entity hooks like
 *         hook_entity_load() are invoked with the entities keyed by numeric id,
 *         while entity-type specific hooks like hook_{entity_type}_load() are
 *         invoked with the entities keyed by name.
 *         Also entity_load() or entity_load_multiple() may be called
 *         with names passed as the $ids parameter, while the results of
 *         entity_load() are always keyed by numeric id. Thus, it is suggested to
 *         make use of entity_load_multiple_by_name() to implement entity-type
 *         specific loading functions like {entity_type}_load_multiple(), as this
 *         function returns the entities keyed by name. 
 *         For exportable entities, it is strongly recommended to make use of a
 *         machine name as names are portable across systems.
 *         This option requires the EntityAPIControllerExportable to work.
 *       - status (optional): The name of the entity property used by the entity
 *         CRUD API to save the exportable entity status using defined bit flags.
 *         Defaults to 'status'. See entity_plus_has_status().
 *       - language (optional): The name of the property, typically 'language', that contains
 *         the language code representing the language the entity has been created
 *         in. This value may be changed when editing the entity and represents
 *         the language its textual components are supposed to have. If no
 *         language property is available, the 'language callback' may be used
 *         instead. This entry can be omitted if the entities of this type are not
 *         language-aware.
 *       - module: (optional) A key for the module property used by the entity CRUD
 *         API to save the source module name for exportable entities that have been
 *         provided in code. Defaults to 'module'.
 *       - default revision: (optional) The name of the entity property used by
 *         the entity CRUD API to determine if a newly-created revision should be
 *         set as the default revision. Defaults to 'default_revision'.
 *         Note that on entity insert the created revision will be always default
 *         regardless of the value of this entity property.
 *    - language callback: (optional) The name of an implementation of
 *      callback_entity_info_language(). In most situations, when needing to
 *      determine this value, inspecting a property named after the 'language'
 *      element of the 'entity keys' should be enough. The language callback is
 *      meant to be used primarily for temporary alterations of the property
 *      value: entity-defining modules are encouraged to always define a
 *      language property, instead of using the callback as main entity language
 *      source. In fact not having a language property defined is likely to
 *      prevent an entity from being queried by language. Moreover, given that
 *      entity_plus_language() is not necessarily used everywhere it would be
 *      appropriate, modules implementing the language callback should be aware
 *      that this might not be always called.
 *    - extra fields controller class (optional): The name of the class that is used to return extra field
 *      information, and for creating display information for extra fields. Extra fields are non-Field API
 *      properties of the entity, other than ID or bundle. This class has to implement
 *      EntityExtraFieldsControllerInterface. Entity Plus provides the default class 
 *      EntityDefaultExtraFieldsController that themes the properties using theme_entity_plus_property(). 
 *    - metadata controller class: (optional) A controller class for providing
 *      entity property info. By default some info is generated out of the
 *      information provided in your hook_schema() implementation, while only read
 *      access is granted to that properties by default. Based upon that the
 *      Entity tokens module also generates token replacements for your entity
 *      type, once activated.
 *      Override the controller class to adapt the defaults and to improve and
 *      complete the generated metadata. Set it to FALSE to disable this feature.
 *      Defaults to the EntityDefaultMetadataController class.
 *    - module (optional): The module providing the entity type. This is optional,
 *      but strongly suggested.
 *    - plural label: (optional) The human-readable, plural name of the entity
 *      type. As 'label' it should start capitalized.
 *    - description: (optional) A human-readable description of the entity type.
 *    - access callback: (optional) Specify a callback that returns access
 *      permissions for the operations 'create', 'update', 'delete' and 'view'.
 *      The callback gets optionally the entity and the user account to check for
 *      passed. See entity_access() for more details on the arguments and
 *      entity_metadata_no_hook_node_access() for an example.
 *    - views controller class: (optional) A controller class for providing views
 *      integration. The given class has to inherit from the class
 *      EntityPlusDefaultViewsController, which is set as default in case the providing
 *      module has been specified (see 'module') and the module does not provide
 *      any views integration. Else it defaults to FALSE, which disables this
 *      feature. See EntityPlusDefaultViewsController.
 *    - creation callback: (optional) A callback that creates a new instance of
 *      this entity type. See entity_plus_metadata_create_node() for an example.
 *    - save callback: (optional) A callback that permanently saves an entity of
 *      this type.
 *    - deletion callback: (optional) A callback that permanently deletes an
 *      entity of this type.
 *    - revision deletion callback: (optional) A callback that deletes a revision
 *      of the entity.
 *    - view callback: (optional) A callback to render a list of entities.
 *      See entity_metadata_view_node() as example.
 *    - form callback: (optional) A callback that returns a fully built edit form
 *      for the entity type.
 *    - token type: (optional) A type name to use for token replacements. Set it
 *      to FALSE if there aren't any token replacements for this entity type.
 *    - configuration: (optional) A boolean value that specifies whether the entity
 *      type should be considered as configuration. Modules working with entities
 *      may use this value to decide whether they should deal with a certain entity
 *      type. Defaults to TRUE to for entity types that are exportable, else to
 *      FALSE.
 *
 * @see entity_load()
 * @see hook_entity_info_alter()
 */

/**
 * This is a placeholder to illustrate the keys added by Entity Plus.
 */
function entity_plus_hook_entity_info() {
  $return = array(
    'basic_entity_plus' => array(
      'label' => t('Basic Entity Plus entity'),
      'plural label' => t('Basic Entity Plus entities'),
      'entity class' => 'BasicEntityPlus',
      'controller class' => 'BasicEntityPlusController',
      'base table' => 'basic_entity_plus',
      'fieldable' => TRUE,
      'entity keys' => array(
        'id' => 'basic_entity_plus_id',
        'bundle' => 'type',
        'label' => 'title',
      ),
      'bundle keys' => array(
        'bundle' => 'type',
      ),
      'bundles' => array(),
      'load hook' => 'basic_entity_plus_load',
      'view modes' => array(),

      'label callback' => 'entity_label',

      // This key is also used by the core entity.tokens.inc to provide a url token.
      'uri callback' => 'basic_entity_plus_uri',

      'module' => 'basic_entity_plus',
      'access callback' => 'basic_entity_plus_access',
    ),
  );

  // Entity to hold bundle definitions.
  $return['basic_entity_plus_type'] = array(
    'label' => t('Basic Entity Plus type'),
    'entity class' => 'BasicEntityPlus',
    'controller class' => 'BasicEntityPlusTypeController',
    'base table' => 'basic_entity_plus_type',
    'fieldable' => FALSE,
    'bundle of' => 'basic_entity_plus',
    'exportable' => TRUE,
    'entity keys' => array(
      'id' => 'id',
      'name' => 'type',
      'label' => 'label',
      'module' => 'module',
    ),
    'module' => 'basic_entity_plus',
    // Enable the admin UI. See Entity UI.
    'admin ui' => array(
      'path' => 'admin/structure/basic_entity_plus-types',
      'file' => 'basic_entity_plus.admin.inc',
      'controller class' => 'EntityDefaultUIController',
    ),
    'access callback' => 'basic_entity_plus_type_access',
    'uri callback' => 'basic_entity_plus_type_uri',
  );

  return $return;
}


/**
 * Allow modules to define metadata about entity properties.
 *
 * Modules providing properties for any entities defined in hook_entity_info()
 * can implement this hook to provide metadata about this properties.
 * For making use of the metadata have a look at the provided wrappers returned
 * by entity_metadata_wrapper().
 * For providing property information for fields see entity_hook_field_info().
 *
 * @return array
 *   An array whose keys are entity type names and whose values are arrays
 *   containing the keys:
 *   - properties: The array describing all properties for this entity. Entries
 *     are keyed by the property name and contain an array of metadata for each
 *     property. The name may only contain alphanumeric lowercase characters
 *     and underscores. Known keys are:
 *     - label: A human readable, translated label for the property.
 *     - description: (optional) A human readable, translated description for
 *       the property.
 *     - type: The data type of the property. To make the property actually
 *       useful it is important to map your properties to one of the known data
 *       types, which currently are:
 *        - text: Any text.
 *        - token: A string containing only lowercase letters, numbers, and
 *          underscores starting with a letter; e.g. this type is useful for
 *          machine readable names.
 *        - integer: A usual PHP integer value.
 *        - decimal: A PHP float or integer.
 *        - date: A full date and time, as timestamp.
 *        - duration: A duration as number of seconds.
 *        - boolean: A usual PHP boolean value.
 *        - uri: An absolute URI or URL.
 *        - entities - You may use the type of each entity known by
 *          hook_entity_info(), e.g. 'node' or 'user'. Internally entities are
 *          represented by their identifieres. In case of single-valued
 *          properties getter callbacks may return full entity objects as well,
 *          while a value of FALSE is interpreted like a NULL value as "property
 *          is not set".
 *        - entity: A special type to be used generically for entities where the
 *          entity type is not known beforehand. The entity has to be
 *          represented using an EntityMetadataWrapper.
 *        - struct: This as well as any else not known type may be used for
 *          supporting arbitrary data structures. For that additional metadata
 *          has to be specified with the 'property info' key. New type names
 *          have to be properly prefixed with the module name.
 *        - list: A list of values, represented as numerically indexed array.
 *          The list<TYPE> notation may be used to specify the type of the
 *          contained items, where TYPE may be any valid type expression.
 *     - bundle: (optional) If the property is an entity, you may specify the
 *       bundle of the referenced entity.
 *     - options list: (optional) A callback that returns a list of possible
 *       values for the property. The callback has to return an array as
 *       used by hook_options_list().
 *       Note that it is possible to return a different set of options depending
 *       whether they are used in read or in write context. See
 *       EntityMetadataWrapper::optionsList() for more details on that.
 *     - getter callback: (optional) A callback used to retrieve the value of
 *       the property. Defaults to entity_property_verbatim_get().
 *       It is important that your data is represented, as documented for your
 *       data type, e.g. a date has to be a timestamp. Thus if necessary, the
 *       getter callback has to do the necessary conversion. In case of an empty
 *       or not set value, the callback has to return NULL.
 *     - setter callback: (optional) A callback used to set the value of the
 *       property. In many cases entity_plus_property_verbatim_set() can be used.
 *     - validation callback: (optional) A callback that returns whether the
 *       passed data value is valid for the property. May be used to implement
 *       additional validation checks, such as to ensure the value is a valid
 *       mail address.
 *     - access callback: (optional) An access callback to allow for checking
 *       'view' and 'edit' access for the described property. If no callback
 *       is specified, a 'setter permission' may be specified instead.
 *     - setter permission: (optional) A permission that describes whether
 *       a user has permission to set ('edit') this property. This permission
 *       is only be taken into account, if no 'access callback' is given.
 *     - schema field: (optional) In case the property is directly based upon
 *       a field specified in the entity's hook_schema(), the name of the field.
 *     - queryable: (optional) Whether a property is queryable with
 *       EntityFieldQuery. Defaults to TRUE if a 'schema field' is specified, or
 *       if the deprecated 'query callback' is set to
 *       'entity_metadata_field_query'. Otherwise it defaults to FALSE.
 *     - query callback: (deprecated) A callback for querying for entities
 *       having the given property value. See entity_property_query().
 *       Generally, properties should be queryable via EntityFieldQuery. If
 *       that is the case, just set 'queryable' to TRUE.
 *     - required: (optional) Whether this property is required for the creation
 *       of a new instance of its entity. See
 *       entity_property_values_create_entity().
 *     - field: (optional) A boolean indicating whether a property is stemming
 *       from a field.
 *     - computed: (optional) A boolean indicating whether a property is
 *       computed, i.e. the property value is not stored or loaded by the
 *       entity's controller but determined on the fly by the getter callback.
 *       Defaults to FALSE.
 *     - entity views field: (optional) If enabled, the property is
 *       automatically exposed as views field available to all views query
 *       backends listing this entity-type. As the property value will always be
 *       generated from a loaded entity object, this is particularly useful for
 *       'computed' properties. Defaults to FALSE.
 *     - sanitized: (optional) For textual properties only, whether the text is
 *       already sanitized. In this case you might want to also specify a raw
 *       getter callback. Defaults to FALSE.
 *     - sanitize: (optional) For textual properties, that are not sanitized
 *       yet, specify a function for sanitizing the value. Defaults to
 *       check_plain().
 *     - raw getter callback: (optional) For sanitized textual properties, a
 *       separate callback which can be used to retrieve the raw, unprocessed
 *       value.
 *     - clear: (optional) An array of property names, of which the cache should
 *       be cleared too once this property is updated. As a rule of thumb any
 *       duplicated properties should be avoided though.
 *     - property info: (optional) An array of info for an arbitrary data
 *       structure together with any else not defined type, see data type
 *       'struct'. Specify metadata in the same way as defined for this hook.
 *     - property info alter: (optional) A callback for altering the property
 *       info before it is used by the metadata wrappers.
 *     - property defaults: (optional) An array of property info defaults for
 *       each property derived of the wrapped data item (e.g. an entity).
 *       Applied by the metadata wrappers.
 *     - auto creation: (optional) Properties of type 'struct' may specify
 *       this callback which is used to automatically create the data structure
 *       (e.g. an array) if necessary. This is necessary in order to support
 *       setting a property of a not yet initialized data structure.
 *       See entity_metadata_field_file_callback() for an example.
 *     - translatable: (optional) Whether the property is translatable, defaults
 *       to FALSE.
 *     - entity token: (optional) If Entity tokens module is enabled, the
 *       module provides a token for the property if one does not exist yet.
 *       Specify FALSE to disable this functionality for the property.
 *   - bundles: An array keyed by bundle name containing further metadata
 *     related to the bundles only. This array may contain the key 'properties'
 *     with an array of info about the bundle specific properties, structured in
 *     the same way as the entity properties array.
 *
 * @see hook_entity_property_info_alter()
 * @see entity_get_property_info()
 * @see entity_metadata_wrapper()
 */
function hook_entity_property_info() {
  $info = array();
  $properties = &$info['node']['properties'];

  $properties['nid'] = array(
    'label' => t("Content ID"),
    'type' => 'integer',
    'description' => t("The unique content ID."),
  );
  return $info;
}

/**
 * Allow modules to alter metadata about entity properties.
 *
 * @see hook_entity_property_info()
 */
function hook_entity_property_info_alter(&$info) {
  $properties = &$info['node']['bundles']['poll']['properties'];

  $properties['poll-votes'] = array(
    'label' => t("Poll votes"),
    'description' => t("The number of votes that have been cast on a poll node."),
    'type' => 'integer',
    'getter callback' => 'entity_property_poll_node_get_properties',
  );
}

/**
 * Provide entity property information for fields.
 *
 * This is a placeholder for describing further keys for hook_field_info(),
 * which are introduced by the entity API.
 *
 * For providing entity property info for fields each field type may specify a
 * property type to map to using the key 'property_type'. With that info in
 * place useful defaults are generated, which suffice for a lot of field
 * types.
 * However it is possible to specify further callbacks that may alter the
 * generated property info. To do so use the key 'property_callbacks' and set
 * it to an array of function names. Apart from that any property info provided
 * for a field instance using the key 'property info' is added in too.
 *
 * @see entity_field_info_alter()
 * @see entity_metadata_field_text_property_callback()
 */
function entity_plus_hook_field_info() {
  return array(
    'text' => array(
      'label' => t('Text'),
      'property_type' => 'text',
      // ...
    ),
  );
}

/**
 * Act after default entities have been rebuilt.
 *
 * This hook is invoked after default entities have been fully saved to the
 * database, but with the lock still active.
 *
 * @param array $entities
 *   An array of the entities that have been saved, keyed by name.
 * @param array $originals
 *   An array of the original copies of the entities that have been saved,
 *   keyed by name.
 *
 * @see _entity_defaults_rebuild()
 */
function hook_ENTITY_TYPE_defaults_rebuild($entities, $originals) {

}

/**
 * Act on an entity before it is about to be created or updated.
 *
 * @param Entity $entity
 *   The entity object.
 * @param string $type
 *   The type of entity being saved (i.e. node, user, comment).
 */
function hook_entity_plus_presave($entity, $type) {
  $entity->changed = REQUEST_TIME;
}

/**
 * Act on entities when inserted.
 *
 * @param Entity $entity
 *   The entity object.
 * @param string $type
 *   The type of entity being inserted (i.e. node, user, comment).
 */
function hook_entity_plus_insert($entity, $type) {
  // Insert the new entity into a fictional table of all entities.
  $info = entity_get_info($type);
  list($id) = entity_extract_ids($type, $entity);
  db_insert('example_entity')
    ->fields(array(
      'type' => $type,
      'id' => $id,
      'created' => REQUEST_TIME,
      'updated' => REQUEST_TIME,
    ))
    ->execute();
}

/**
 * Act on entities when updated.
 *
 * @param Entity $entity
 *   The entity object.
 * @param sting $type
 *   The type of entity being updated (e.g. node, user, comment).
 */
function hook_entity_plus_update($entity, $type) {
  // Update the entity's entry in a fictional table of all entities.
  $info = entity_get_info($type);
  list($id) = entity_extract_ids($type, $entity);
  db_update('example_entity')
    ->fields(array(
      'updated' => REQUEST_TIME,
    ))
    ->condition('type', $type)
    ->condition('id', $id)
    ->execute();
}

/**
 * Respond to entity deletion.
 *
 * This hook runs after the entity type-specific delete hook.
 *
 * @param Entity $entity
 *   The entity object for the entity that has been deleted.
 * @param string $type
 *   The type of entity being deleted (i.e. node, user, comment).
 */
function hook_entity_plus_delete($entity, $type) {
  // Delete the entity's entry from a fictional table of all entities.
  $info = entity_get_info($type);
  list($id) = entity_extract_ids($type, $entity);
  db_delete('example_entity')
    ->condition('type', $type)
    ->condition('id', $id)
    ->execute();
}

/**
 * Act on an entity that is being assembled before rendering.
 *
 * The module may add elements to $entity->content prior to rendering. This hook
 * will be called after hook_view(). The structure of $entity->content is a
 * renderable array as expected by backdrop_render().
 *
 * When $view_mode is 'rss', modules can also add extra RSS elements and
 * namespaces to $node->rss_elements and $node->rss_namespaces respectively for
 * the RSS item generated for this node.
 * For details on how this is used, see node_feed().
 *
 * @param Entity $entity
 *   The entity that is being assembled for rendering.
 * @param string $entity_type
 *   The entity type of the entity being assembled.
 * @param string $view_mode
 *   The $view_mode parameter from entity_view().
 * @param string $langcode
 *   The language code used for rendering.
 *
 * @see hook_entity_view()
 * @see hook_entity_view_alter()
 * @see hook_entity_plus_view_alter()
 *
 * @ingroup node_api_hooks
 */
function hook_entity_plus_view($entity, $entity_type, $view_mode, $langcode) {
  $entity->content['my_additional_field'] = array(
    '#markup' => $additional_field,
    '#weight' => 10,
    '#theme' => 'mymodule_my_additional_field',
  );
}

/**
 * Alter the handlers used by the data selection tables provided by this module.
 *
 * @param array $field_handlers
 *   An array of the field handler classes to use for specific types. The keys
 *   are the types, mapped to their respective classes. Contained types are:
 *   - All primitive types known by the entity API (see
 *     hook_entity_property_info()).
 *   - options: Special type for fields having an options list.
 *   - field: Special type for Field API fields.
 *   - entity: Special type for entity-valued fields.
 *   - relationship: Views relationship handler to use for relationships.
 *   Values for all specific entity types can be additionally added.
 *
 * @see entity_plus_views_field_definition()
 * @see entity_plus_views_get_field_handlers()
 */
function hook_entity_plus_views_field_handlers_alter(array &$field_handlers) {
  $field_handlers['text'] = 'example_text_handler';
}
