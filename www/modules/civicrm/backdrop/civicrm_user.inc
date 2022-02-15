<?php

/**
 * @file
 * Project:     CiviCRM: Constituent Relationship Management for NP's
 * File:        civicrm_user.inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * Backdrop module include file.
 */

/**
 * User hooks for civicrm module
 *
 * Note that we ignore the edit field and do not follow the backdrop protocol
 * of extracting values from the edit field. We extract the fields directly
 * from $_POST. This is because the underlying form package that we use
 * (HTML_QuickForm) does the form value extraction and validation.
 *
 * @abstractparam array  $edit     The array of form values submitted by the user.
 *
 * @param object $account     The user object on which the operation is being performed.
 * @param object $profile The active profile of user information being edited.
 *
 * @return mixed           depends on the operation being performed
 */

/**
 * Implements hook_user_login().
 */
function civicrm_user_login(&$edit, $account) {
  if (!civicrm_initialize()) {
    return;
  }
  return CRM_Core_BAO_UFMatch::synchronize($account, FALSE, 'Backdrop',
    civicrm_get_ctype('Individual')
  );
}

/**
 * Implements hook_user_insert().
 */
function civicrm_user_insert($account) {
  if (!civicrm_initialize()) {
    return;
  }

  $userSystem = CRM_Core_Config::singleton()->userSystem;
  $userSystemID = $userSystem->getBestUFID($account);
  $uniqId = $userSystem->getBestUFUniqueIdentifier($account);
  CRM_Core_BAO_UFMatch::synchronizeUFMatch($account, $userSystemID, $uniqId, 'Backdrop', NULL, 'Individual', $isLogin = FALSE);

  // As per CRM-7858, the email address in CiviCRM isn't always set
  // with a call to synchronize(). So we force this through.
  $contact_id = CRM_Core_BAO_UFMatch::getContactId($account->id());
  if (empty($contact_id)) {
    return;
  }
  CRM_Core_BAO_UFMatch::updateContactEmail($contact_id, trim($account->mail));

  // If already in CiviCRM, we don't do then next bit because it will cause
  // the current request to exit, stopping execution.
  $config = CRM_Core_Config::singleton();
  if ($config->inCiviCRM) {
    return;
  }

  // Determine if the user is on a CiviCRM generated page or user hook.
  // If the form have some civicrm unique token then skip.
  if (!isset($_POST['_qf_default'])) {
    return;
  }

  // Process any profile form fields that may have been submitted.
  // In particular, this will pick up form fields that were submitted on the user_registration page.
  CRM_Core_BAO_UFGroup::getEditHTML($contact_id, '', 2, TRUE, FALSE, NULL, FALSE, civicrm_get_ctype('Individual'));

}

/**
 * Implements hook_user_update().
 */
function civicrm_user_update($account) {
  if (!civicrm_initialize()) {
    return;
  }
  // This always comes in via user hook.
  // in D7 we don't know if the email has changed, so we go ahead and update
  if (isset($account->mail) && !empty($account->mail)) {
    $contactID = CRM_Core_BAO_UFMatch::getContactId($account->uid);
    // cant find the contactID, so lets skip
    if (!$contactID) {
      return;
    }
    $contactEmail = CRM_Contact_BAO_Contact::getPrimaryEmail($contactID);
    $userEmail = trim($account->mail);
    if ($contactEmail != $userEmail) {
      CRM_Core_BAO_UFMatch::updateContactEmail($contactID, $userEmail);
    }

    // reset navigation on user role change
    $editRoles     = array_keys(CRM_Utils_Array::value('roles', $account, array()));
    $orginRoles    = array_keys($account->original->roles);
    $editRoleDiff  = array_diff($editRoles, $orginRoles);
    $orginRoleDiff = array_diff($orginRoles, $editRoles);
    if (!empty($editRoleDiff) || !empty($orginRoleDiff)) {
      CRM_Core_BAO_Navigation::resetNavigation($contactID);
    }
  }
}

/**
 * Implements hook_user_delete().
 */
function civicrm_user_delete($account) {
  if (!civicrm_initialize()) {
    return;
  }
  CRM_Core_BAO_UFMatch::deleteUser($account->uid);
}

/**
 * Get Profiles for User Account
 * @return array $ufGroups
 */
function civicrm_user_profiles() {
  if (!civicrm_initialize()) {
    return array();
  }

  $allUFGroups = CRM_Core_BAO_UFGroup::getModuleUFGroup('User Account', 0, TRUE, CRM_Core_Permission::VIEW, array('id', 'name', 'title', 'is_active'));
  $ufGroups = array();

  foreach ($allUFGroups as $key => $value) {
    if ($value['is_active']) {
      $name = $value['name'];
      $ufGroups[] = array(
        'id' => $key,
        'name' => $name,
        'title' => $value['title'],
      );
    }
  }

  return $ufGroups;
}

/**
 * Implements hook_user_view().
 *
 * @todo I suspect that some of the stuff done in the old form_alter handler
 *       should live here instead under D7
 */
function civicrm_user_view($account, $view_mode, $langcode) {
  if (!civicrm_initialize()) {
    return;
  }
  $userID = CRM_Core_BAO_UFMatch::getContactId($account->uid);
  if ($userID) {
    // Make sure user has permission to view the record.
    $contactURL = NULL;
    $civiPerm = CRM_Contact_BAO_Contact_Permission::allow($userID);

    if (CRM_Core_Permission::check('access CiviCRM') && $civiPerm) {
      $contactURL
        = '<div class="form-item form-type-item user-page-link" id="user-page-contact">'
        . l(ts("View contact record"),
        'civicrm/contact/view',
          array('query' => array('reset' => 1, 'cid' => $userID))
        ) .
        '</div>';
    }

    if (CRM_Core_Permission::check('access Contact Dashboard')) {
      $contactURL .=
        '<div class="form-item form-type-item user-page-link" id="user-page-dashboard">'
        . l(ts("View Contact Dashboard"),
          'civicrm/user',
          array('query' => array('reset' => 1, 'id' => $userID))
        ) .
        '</div>';
    }

    $ctype = CRM_Core_DAO::getFieldValue('CRM_Contact_DAO_Contact', $userID, 'contact_type');
    $ufGroups = CRM_Core_BAO_UFGroup::getModuleUFGroup('User Account', 0, TRUE, CRM_Core_Permission::VIEW, array('id', 'name', 'title', 'is_active'));
    $weight = 100;
    foreach ($ufGroups as $id => $ufGroup) {

      $fieldType = CRM_Core_BAO_UFField::getProfileType($id);
      if (CRM_Contact_BAO_ContactType::isaSubType($fieldType)) {
        $fieldType = CRM_Contact_BAO_ContactType::getBasicType($fieldType);
      }
      if (($fieldType != 'Contact') && ($fieldType != $ctype)) {
        continue;
      }
      $page = new CRM_Profile_Page_Dynamic($userID, $id, NULL, TRUE);
      $pageContent = $page->run();
      // CRM-3537: profile edit link
      $editURL = '';
      if (user_edit_access($account)) {
        $editURL = '<span class="user-page-link" id="user-page-profile-' . substr($ufGroup['title'], 0, 3) . '" ><span class="user-page-bullet">&raquo;</span> '
          . l(ts("Edit %1", array(1 => $ufGroup['title'])), "user/{$account->uid}/edit/" . $ufGroup['name']) . '</span>';
      }

      if ($pageContent) {
        $account->content[$ufGroup['name']] = array(
          '#type' => 'item',
          '#title' => $ufGroup['title'],
          '#value' => $pageContent . $editURL,
          '#markup' => $pageContent . $editURL,
          '#weight' => $weight,
        );

        $weight += 10;
      }
    }
    if ($contactURL) {
      $account->content['urls'] = array(
        '#type' => 'item',
        '#title' => t('CRM Contact'),
        '#markup' => $contactURL,
        '#weight' => $weight,
      );
    }
  }
}

/**
 * Implements hook_user_logout().
 */
function civicrm_user_logout($account) {
  if (!civicrm_initialize()) {
    return;
  }
  $session = CRM_Core_Session::singleton();
  $session->reset();
}
