<?php
/**
 * @file
 * API documentation for hooks provided by the Project module.
 */

/**
 * Return information about possible project 'behavior' options.
 *
 * There is a top-level set of radio buttons to determine how each node type
 * behaves with respect to the Project* system (is it a project, an issue, a
 * release, etc). This info hook allows modules to advertise possible choices
 * for the project behavior for each node type.
 *
 * If there are any module-specific settings that need to happen once we know
 * how a node type should behave for Project*, those should be defined in a
 * callback function that's advertised here. The spot where this callback is
 * invoked will automatically populate the #states value for these form
 * elements to only appear if the behavior setting radio for this module is
 * selected. Since this is all ultimately impacting a node_type edit form, the
 * keys of these form elements will be used to automatically call
 * variable_set() with the node type machine name appended as a suffix. The
 * settings callback will get the node type machine name as a parameter.
 *
 * @return array
 *   An array of information about each possible project behavior with the
 *   following keys:
 *   - 'machine name': The machine name for the behavior.
 *   - 'label': The human readable label for the behavior.
 *   - 'settings callback': Optional function to invoke to provide
 *      behavior-specific settings.
 *
 * @see node_type_form_submit()
 */
function hook_project_behavior_info() {
  return array(
    'machine name' => 'project_milestone',
    'label' => t('Used for project milestones'),
    'settings callback' => 'project_milestone_behavior_settings',
  );
}
