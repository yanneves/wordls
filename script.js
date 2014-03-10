(function(angular) {
	'use strict';

	// instantiate WordLS Angular Module
	var app = angular.module('wordls', []);

	app.constant('LETTERVAL', {A:1,B:3,C:3,D:2,E:1,F:4,G:2,H:4,I:1,J:8,K:5,L:1,M:3,N:1,O:1,P:3,Q:10,R:1,S:1,T:1,U:1,V:4,W:4,X:8,Y:4,Z:10});

	// WordLS Service
	app.service('wordService', ['$http', 'LETTERVAL', function($http, LETTERVAL) {
		// base url for searches
		var urlBase = 'http://aaunel.com/cgi-bin/word.pl';

		/**
		 *	Search call to retrieve matching words.
		 *	@params {String} rack
		 *	@params {String} pattern
		 *	@returns {Promise} $http
		 */
		this.getWords = function(rack, pattern) {
			return $http.get(urlBase, {
				params: {
					rack: rack,
					pattern: pattern
				}
			});
		};

		/**
		 *	Calculate basic word value in Scrabble rules.
		 *	@params {String} word
		 *	@returns {Number} Value
		 */
		this.getWordValue = function(word) {
			var val = 0;
			word.split('').forEach(function(letter) {
				val += LETTERVAL[letter] || 0;
			});
			return val;
		};
	}]);

	// WordLS Controller
	app.controller('wordController', ['$scope', 'wordService', function($scope, service) {
		// preserve input
		$scope.input = {rack: "", pattern: ""};
		// cache for results
		$scope.res = {words: [], state: 0};
		// set defaults for reset
		var _defaults = {
			input: angular.copy($scope.input),
			res: angular.copy($scope.res)
		};

		// number of results
		$scope.__defineGetter__('found', function() {
			return $scope.res.words.length + "";
		});

		/**
		 *	Fetches results from user query.
		 *	@params $event Angular Event
		 *	@params {String} rack User input - letter rack
		 *	@params {String} pattern User input - board pattern
		 *	@public
		 */
		$scope.search = function($event, rack, pattern) {
			// search rack-pattern combination for matches,
			// communicating with service layer
			service.getWords(rack, pattern)
				.success(function(res) {
					$scope.res.state = 200;
					res.words.forEach(function(word) {
						$scope.res.words.push({
							word: word,
							value: service.getWordValue(word)
						});
					});
				});

			// prevent fallback form submission
			$event.preventDefault();
		};

		/**
		 *	Clears results.
		 *	@public
		 */
		$scope.back = function() {
			$scope.res = angular.copy(_defaults.res);
		};

		/**
		 *	Clears results and resets form.
		 *	@public
		 */
		$scope.reset = function() {
			$scope.input = angular.copy(_defaults.input);
			$scope.back();
		};
	}]);
	
})(angular);
