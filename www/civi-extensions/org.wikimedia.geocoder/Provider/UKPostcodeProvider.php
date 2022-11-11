<?php

/*
 * This file works with the Geocoder package.
 *
 * @author Rich Lott / Artful Robot <forums@artfulrobot.uk>
 */

namespace Geocoder\Provider;

use Civi;
use Geocoder\Collection;
use Geocoder\Exception\UnsupportedOperation;
use Geocoder\Model\AddressBuilder;
use Geocoder\Model\AddressCollection;
use Geocoder\Query\GeocodeQuery;
use Geocoder\Query\ReverseQuery;
use Geocoder\Provider\AbstractProvider;
use Geocoder\Provider\Provider;
use Geocoder\Exception\CollectionIsEmpty;

final class UKPostcodeProvider extends AbstractProvider implements Provider
{
  /**
   * @param HttpClient $dummy (unused)
   *
   * @throws \Exception
   */
    public function __construct($dummy) {
    }

    /**
     * {@inheritdoc}
     */
    public function geocodeQuery(GeocodeQuery $query): Collection {

      if (!isset(Civi::$statics[__CLASS__])) {
        Civi::$statics[__CLASS__] = (bool) (\CRM_Core_DAO::singleValueQuery("SHOW TABLES LIKE 'civicrm_open_postcode_geo_uk'"));
      }
      if (!Civi::$statics[__CLASS__]) {
        // We don't have the data table available.
        throw new CollectionIsEmpty();
      }

      $postcodeNoSpace = preg_replace('/ +/', '', $query->getText());

      $sql = "
         SELECT *
         FROM civicrm_open_postcode_geo_uk
         WHERE postcode_no_space = %1";

      $result = \CRM_Core_DAO::executeQuery(
        $sql,
        [1 => [$postcodeNoSpace, 'String']]
      );

      $builder = new AddressBuilder($this->getName());
      if ($result->fetch()) {
        $builder->setCoordinates($result->latitude, $result->longitude);
        $builder->setPostalCode($result->postcode);
        return new AddressCollection([$builder->build()]);
      }

      throw new CollectionIsEmpty();
    }

    /**
     * {@inheritdoc}
     */
    public function reverseQuery(ReverseQuery $query): Collection
    {
        throw new UnsupportedOperation('The data table provider is not able to do reverse geocoding yet.');
    }

    /**
     * {@inheritdoc}
     */
    public function getName(): string {
      return 'uk_postcode';
    }
}
