Geocoding for CiviCRM based on geocoder library

Requires - CiviCRM 5.28, php 7.1

Implementation of geocoder library (which itself supports multiple providers) https://github.com/geocoder-php/mapquest-provider. Only those that have been tested are enabled so far.

When an address is edited CiviCRM will obtain additional data from the geocoding provider
and save it to the `civicrm_address` database with the address. It will also geocode addresses
to be used as the focal point of proximity searches or for event maps.

Note that the terms of data use by geocoding providers varies and it is your responsibility
to understand and adhere to them.

Currently enabled geocoders are

- Open Street Maps - this is zero-config & is enabled as the default (lowest weight)on install if you have no existing mapping provider
- USZipGeocoder - this is enabled on install & has no config. It will work as a fallback for US addresses only.
- UK Postcodes - see below
- MapQuest - requires an API key to be used
- GoogleMaps - requires an API key to be used - this is enabled on install as the default if you
already have google configured as your provider. However the Terms of service suggest it may not be a good choose https://support.google.com/code/answer/55180?hl=en
- GeoName DB geocoder - this requires that you get a sample dataset from geonames. I will require a developer or similar to tweak the download into an sql table. There is a sample dataset for New Zealand in the install directory & if loaded it will work for New Zealand.
- Here (not enabled by default)
- Addok (not enabled by default)

Features

- Threshold standdown period. If the geocoding quota is hit for a provider it is not used
again until the standdown has expired. By default the standdown is 1 minute but it is configurable per geocoder instance.
- Provider fall over. If a provider is not valid (e.g because the quota was hit or it only does a
 particular country or it's required fields are not present) then the next geocoder (by weight)
 will be used.
- Database table based geocoding. If you do not wish to interact with an external site then
a US zip table lookup is available (from CiviSpace). It is possible to download other datasets (e.g from geonames) & upload & use them but that requires some more config.
- Datafill fields. Each geocoder is configured with 2 sets of fields to retain - 'retained_response_fields' - these overwrite the existing fields for the address - usually latitude & longitude
  'data fill fields' - these are added to the existing fields if the existing field is not set.
- other providers from https://github.com/geocoder-php/Geocoder#providers can be added easily


As of the 1.4 release there are some metadata & field use changes. These are
best illustrated by examples.

 - the arguments that can be defined in the metadata (to instantiate the provider class) are expanded
e.g in Open Street map the following
```
    'metadata' => [
      'argument' => ['geocoder.url', 'server.User-Agent:CiviCRM', 'server.Referrer'],
  ],
```

means to pass
1) geocoder metadata parameter (in this case from the field) 'url'
2) $_SERVER parameter 'User-Agent' with a default of 'CiviCRM'
3) $_SERVER parameter 'Referrer'

For the US ZIP Geocoder
```
      'argument' => ['pass_through' => [
        'tableName' => 'civicrm_geocoder_zip_dataset',
        'columns' => ['city', 'state_code', 'latitude', 'longitude', 'timezone'],
      ]],
```
means to pass
1) The value under the key 'pass_through' (or any key starting with that string) directly.

For the Here provider
```
    'metadata' => [
      'argument' => ['api_key.app_id', 'api_key.app_code'],
```
means to pass
1) app_id from the json_encoded array stored in the DB field api_key.
2) app_code from the json_encoded array stored in the DB field api_key.

Also from 1.4 the api_key field is used for a flat parameter - ie
api_key = xyz
or multipart user data - ie
api_key = {'app_id' : 'xy', 'app_code' : 'z'}

Next steps

1) make geocoders configurable - the form at /civicrm/a/#/geocoders
currently only gives view access. I'm committed to extending the form based on the metadata rather than hard-coding & have added 'help_text' & 'user_editable_fields' to the entity specs. The plan is to expose these via getfields & then use them to drive the form. I'd like a cool way to manage weight.

2) consider caching - for https providers - https://github.com/geocoder-php/Geocoder/blob/master/docs/cookbook/cache.md
For DB providers we could cache the result of each query. The downside is potential
large memory usage for little query gain if there is a big spread of postal codes.


Also of interest
- library supports ip address geocoding

## UK Postcode geocoder

- This *only* goes on postcodes which it looks up from a database (so no online service, no fees, limits or latency).

- It can handle *and correct* postcodes with spaces missing/in wrong places.

- It is not installed by default because of the size of the data.

### Data License

Due to licensing, only GB's postcodes are included, not the UK's. (i.e. no
Northern Ireland postcodes are included.) You may be permitted to add NI
postcodes to your local database, if you can get them, but they can't be
distributed as part of this extension.

The data came from https://www.getthedata.com/open-postcode-geo

> Open Postcode Geo is derived from the ONS Postcode Directory.
>
> From the ONS:
>
> http://www.ons.gov.uk/methodology/geography/licences
>
> Our postcode products (derived from Code-Point(R) Open) are subject to the Open Government Licence and the Ordnance Survey OpenData Licence.
>
> - Contains OS data (c) Crown copyright and database right 2021
> - Contains Royal Mail data (c) Royal Mail copyright and database right 2021
> - Contains National Statistics data (c) Crown copyright and database right 2021

### Downloading the data

1. grab the .sql.gz file from https://www.getthedata.com/open-postcode-geo and unzip it.
2. Edit the table name to `civicrm_open_postcode_geo_uk` (you can do this with `sed -i 's/open_postcode_geo/civicrm_open_postcode_geo_uk/g' /path/to/open_postcode_geo.sql`)
3. Import the data into your CiviCRM database
4. Run SQL like this:
    ```sql
    ALTER TABLE civicrm_open_postcode_geo_uk
    DROP `status`,
    DROP usertype,
    DROP easting,
    DROP northing,
    DROP positional_quality_indicator,
    DROP country,
    DROP postcode_fixed_width_seven,
    DROP postcode_fixed_width_eight,
    DROP postcode_area,
    DROP postcode_district,
    DROP postcode_sector,
    DROP outcode,
    DROP incode,
    DROP KEY IF EXISTS postcode_no_space,
    ADD PRIMARY KEY (postcode_no_space);
    ```
5. Enable the UK Postcode geocoder. Not sure if there's a UI for this, but you can do it via the API (v3).
