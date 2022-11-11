(function(angular, $, _) {

  var entityName = 'Geocoder';
  angular.module('geocoder').config(function($routeProvider) {
      $routeProvider.when('/geocoders', {
        controller: 'GeocodereditCtrl',
        templateUrl: '~/geocoder/editCtrl.html',

        // If you need to look up data when opening the page, list it out
        // under "resolve".
        resolve: {
          entities: function(crmApi) {
            return crmApi('Geocoder', 'get', {
            });
          },
          fieldSpec: function(crmApi) {
            return crmApi(entityName, 'getfields', {
              'options' : {'get_options' : true}
            });
          }
        }
      });
    }
  );

  // The controller uses *injection*. This default injects a few things:
  //   $scope -- This is the set of variables shared between JS and HTML.
  //   crmApi, crmStatus, crmUiHelp -- These are services provided by civicrm-core.
  //   entities -- The current entities assigned to the page, defined above in config().
  angular.module('geocoder').controller('GeocodereditCtrl', function($scope, crmApi, crmStatus, crmUiHelp, entities, fieldSpec) {
    // The ts() and hs() functions help load strings for this module.
    var ts = $scope.ts = CRM.ts('geocoder');
    var hs = $scope.hs = crmUiHelp({file: 'CRM/geocoder/editCtrl'}); // See: templates/CRM/geocoder/editCtrl.hlp

    $scope.entities = entities['values'];
    $scope.fields = fieldSpec['values'];

    $scope.save = function save() {
      return crmStatus(
        // Status messages. For defaults, just use "{}"
        {start: ts('Saving...'), success: ts('Saved')},
        // The save action. Note that crmApi() returns a promise.
        crmApi('Contact', 'create', {
          id: myContact.id,
          first_name: myContact.first_name,
          last_name: myContact.last_name
        })
      );
    };
  });

})(angular, CRM.$, CRM._);
