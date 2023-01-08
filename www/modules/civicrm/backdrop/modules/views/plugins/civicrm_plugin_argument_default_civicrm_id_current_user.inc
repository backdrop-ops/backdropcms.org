<?php
/*
 +--------------------------------------------------------------------+
 | CiviCRM version 5                                                  |
 +--------------------------------------------------------------------+
 | This file is a part of CiviCRM.                                    |
 |                                                                    |
 | CiviCRM is free software; you can copy, modify, and distribute it  |
 | under the terms of the GNU Affero General Public License           |
 | Version 3, 19 November 2007 and the CiviCRM Licensing Exception.   |
 |                                                                    |
 | CiviCRM is distributed in the hope that it will be useful, but     |
 | WITHOUT ANY WARRANTY; without even the implied warranty of         |
 | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.               |
 | See the GNU Affero General Public License for more details.        |
 |                                                                    |
 | You should have received a copy of the GNU Affero General Public   |
 | License and the CiviCRM Licensing Exception along                  |
 | with this program; if not, contact CiviCRM LLC                     |
 | at info[AT]civicrm[DOT]org. If you have questions about the        |
 | GNU Affero General Public License or the licensing of CiviCRM,     |
 | see the CiviCRM license FAQ at http://civicrm.org/licensing        |
 +--------------------------------------------------------------------+
 */

/**
 * @file Contains a plugin to return the CiviCRM ID from the current user.
 */

/**
 * Class civicrm_plugin_argument_default_civicrm_id_current_user
 */
class civicrm_plugin_argument_default_civicrm_id_current_user extends views_plugin_argument_default {

  /**
   * Get the default argument (the CiviCRM ID) or return 0.
   */
  public function get_argument() {
    global $user;
    civicrm_initialize();
    $contact_id = 0;
    $uFMatches = \Civi\Api4\UFMatch::get(FALSE)
      ->addSelect('contact_id')
      ->addWhere('uf_id', '=', $user->uid)
      ->setLimit(1)
      ->execute();
    foreach ($uFMatches as $uFMatch) {
      if (!empty($uFMatch['contact_id'])) {
        $contact_id = $uFMatch['contact_id'];
      }
    }

    return $contact_id;
  }

}
