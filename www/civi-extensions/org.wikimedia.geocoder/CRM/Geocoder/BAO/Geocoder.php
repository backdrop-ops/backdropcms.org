<?php
use CRM_Geocoder_ExtensionUtil as E;

class CRM_Geocoder_BAO_Geocoder extends CRM_Geocoder_DAO_Geocoder {

  /**
   * Create a new Geocoder based on array-data
   *
   * @param array $params key-value pairs
   * @return CRM_Geocoder_DAO_Geocoder|NULL
   *
  public static function create($params) {
    $className = 'CRM_Geocoder_DAO_Geocoder';
    $entityName = 'Geocoder';
    $hook = empty($params['id']) ? 'create' : 'edit';

    CRM_Utils_Hook::pre($hook, $entityName, CRM_Utils_Array::value('id', $params), $params);
    $instance = new $className();
    $instance->copyValues($params);
    $instance->save();
    CRM_Utils_Hook::post($hook, $entityName, $instance->id, $instance);

    return $instance;
  } */

}
