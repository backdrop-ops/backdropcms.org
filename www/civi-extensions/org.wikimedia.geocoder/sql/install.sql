CREATE TABLE `civicrm_geocoder` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Unique Geocoder ID',
  `name` varchar(32) NOT NULL COMMENT 'Provider name',
  `title` varchar(32) NOT NULL COMMENT 'Provider Title',
  `class` varchar(32) NOT NULL DEFAULT '' COMMENT 'Non generic part of the class name - after Geocoder\\Provider\\. See mgd files for examples',
  `is_active` tinyint(1) DEFAULT '0' COMMENT 'Enabled?',
  `weight` int(10) unsigned DEFAULT NULL COMMENT 'Weight',
  `api_key` varchar(255) DEFAULT NULL COMMENT 'API Key',
  `url` varchar(255) DEFAULT NULL COMMENT 'URL (if required)',
  `required_fields` varchar(255) DEFAULT NULL COMMENT 'json array of fields required for this to parse',
  `retained_response_fields` varchar(255) DEFAULT '["geo_code_1","geo_code_2"]' COMMENT 'fields to be retained from the response',
  `datafill_response_fields` varchar(255) DEFAULT NULL COMMENT 'fields retained to fill but not overwrite data',
  `threshold_standdown` int(11) NOT NULL DEFAULT '60' COMMENT 'Number of seconds to wait before retrying after hitting threshold. Geocaching disabled in this time',
  `threshold_last_hit` timestamp NULL DEFAULT NULL COMMENT 'Timestamp when the threshold was last hit.',
  `valid_countries` varchar(255) DEFAULT NULL COMMENT 'Countries this geocoder is valid for',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=5;
