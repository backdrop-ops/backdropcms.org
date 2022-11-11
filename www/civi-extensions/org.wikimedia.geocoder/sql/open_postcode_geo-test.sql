-- Test data, one row.
-- See README for getting the full data.
DROP TABLE IF EXISTS `civicrm_open_postcode_geo_uk`;
CREATE TABLE `civicrm_open_postcode_geo_uk` (
  `postcode` char(8) NOT NULL PRIMARY KEY,
  `latitude` decimal(9,6) DEFAULT NULL,
  `longitude` decimal(9,6) DEFAULT NULL,
  `postcode_no_space` char(7) NOT NULL,
  UNIQUE KEY `postcode_no_space` (`postcode_no_space`)
) ENGINE=InnoDB;
INSERT INTO `civicrm_open_postcode_geo_uk` VALUES ('SW1A 0AA',51.499840,-0.124663,'SW1A0AA');

