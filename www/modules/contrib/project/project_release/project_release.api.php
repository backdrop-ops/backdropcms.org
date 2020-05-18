<?php
/**
 * @file
 * API documentation for hooks provided by the Project Release module.
 */

/**
 * Alter the XML for a release of a contrib project.
 *
 * The Project module defines a Project content type that defines a contrib
 * project, such as a module, theme, or layout. The Project Release module
 * defines a Project Release content type that describes one individual release
 * of that contrib project, such as My Module 1.x-1.0.
 *
 * Project Release publishes metadata about all released modules in XML format.
 * Sites use this metadata for both the Available Updates section of the site
 * and to create a catalog of available modules for installation. To generate
 * the XML for a contrib project, the Project Release module fetches all of the
 * Project Release nodes for a given Project node, and uses them to create an
 * array with the metadata for each release.
 *
 * This hook is called as Project Release is creating that list of releases for
 * a contrib project. The $release_xml contains the metadata for a single
 * release of the contrib project as described by the Project Release node and
 * the Project node.
 *
 * The $release_xml will be rendered into XML by format_xml_elements(). It
 * decribes a single <release> element in the final XML.
 *
 * To edit the full list at once, or metadata about the contrib project itself,
 * implement hook_project_release_project_xml_alter() instead.
 *
 * @param &$release_xml
 *   An array describing the metadata for this particular release of a project.
 * @param $relelase_node
 *   The Project Release node for this release.
 * @param $project_node
 *   The Project node for this module.
 * @see format_xml_elements
 */
function hook_project_release_release_xml_alter(&$release_xml, $relelase_node, $project_node) {
  if ($release_node->comment_count) {
    $release_xml['comment_count'] = $release_node->comment_count;
  }
}

/**
 * Alter the XML for a project.
 *
 * The Project module defines a Project content type that defines a contrib
 * project, such as a module, theme, or layout.
 *
 * This hook is called as Project Release is getting ready to write out the XML
 * of a contrib project for a given Backdrop API release.  At this point,
 * Project Release has already created a list of contrib project's releases
 * (e.g. 1.x-1.0, 1.x-1.1-rc2) and is now creating the metadata about the
 * contrib project itself.
 *
 * $project_xml will include all of the pieces that have already passed through
 * hook_project_release_release_xml_alter() and Project-level metadata, such as
 * the module's human-friendly name and machine name, supported major versions,
 * and the recommended major version of the module.
 *
 * The $project_xml array will be rendered into XML by format_xml_elements().
 *
 * @param &$project_xml
 *   An array describing the metadata for the project for a given API version.
 * @param $project_node
 *   The Project node.
 * @param $release_version_api
 *   The Backdrop API version for the release.  E.g., '1.x', '2.x', 'all'.
 * @see format_xml_elements
 */
function hook_project_release_project_xml_alter(&$project_xml, $project_node, $release_version_api) {
  if ($project_node->project['sandbox']) {
    $project_xml['sandbox'] = 'true';
  }
}

/**
 * Alter the XML for the list of contrib projects.
 *
 * The Project Release module publishes a list of all contrib projects (modules,
 * themes, etc.) available on the site in a single XML file. This file lists
 * only information about the contrib projects themselves. There is no
 * information about individiual releases included here.
 *
 * The $project_xml array will be rendered into XML by format_xml_elements(). It
 * describes a single <project> element in the final XML.
 *
 * @param &$project_xml
 *   An array describing the metadata for the contrib project.
 * @param $project_node
 *   The Project node for this contrib project.
 * @param $project_api_versions
 *   An array of the supported Backdrop API versions of this contrib module.
 *   E.g., array('1.x', '2.x').
 * @see format_xml_elements
 */
function hook_project_release_project_list_xml_alter(&$project_xml, $project_node, $project_api_versions) {
  if ($project_node->project['sandbox']) {
    $project_xml['sandbox'] = 'true';
  }
}
