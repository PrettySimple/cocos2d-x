/* CCApplication-emscripten.src.js */ /* UTF8-Côôkie */

/*
	Detects visibility changes, mimics native applications behavior.

	Whenever we detect the window is not visible, we're triggering the following AppDelegate calls:

		applicationDidEnterBackground()
		applicationDidBecomeInactive()

	Whenever we detect the window is visible again;

		applicationWillEnterForeground()
		applicationDidBecomeActive()

	The implementation is partially based on:

		https://stackoverflow.com/questions/1060008/is-there-a-way-to-detect-if-a-browser-window-is-not-currently-active

	Yet, we only handle the cases where the visibility can actually be known straight away (not only in an event handler).

*/



'use strict';

(function()
{
	// Detect the supported implementation, if any

	const impl = (function()
	{
		// Standard (recommendation): https://www.w3.org/TR/page-visibility/

		if('visibilityState' in document)
			return { event: 'visibilitychange', inForeground: function() { return document.visibilityState == 'visible'; } };

		// "Modern" fallbacks (with the ability to get the state synchronously)

		if('hidden' in document)
			return { event: 'visibilitychange',  inForeground: function() { return !document.hidden; } };

		if('mozHidden' in document)
			return { event: 'mozvisibilitychange',  inForeground: function() { return !document.mozHidden; } };

		if('webkitHidden' in document)
			return { event: 'webkitvisibilitychange',  inForeground: function() { return !document.webkitHidden; } };

		if('msHidden' in document)
			return { event: 'msvisibilitychange',  inForeground: function() { return !document.msHidden; } };

		// There are fallbacks using events (see: https://stackoverflow.com/questions/1060008/is-there-a-way-to-detect-if-a-browser-window-is-not-currently-active),
		// yet they don't allow getting a synchronous feedback - which is annoying as we want to match each pair of (onBackground,onInactive) with (onActive,onForeground).

		return null;
	})();

	// If no modern implementation, don't support the feature at all
	if(impl === null)
		return;

	(function()
	{
		const _binding = Module.cocos_Application_binding;	// TODO: rename this (and all other) EM bindings...

		// Check the initial state

		let	inForeground = impl.inForeground();

		/*<DEBUG>*/
		console.log('*** Application initially in foreground: '+(inForeground ? 'YES' : 'NO'));
		/*</DEBUG>*/

		if(!inForeground)
		{
			// Oops, we're starting in background... immediately notify the application
			_binding.onBackground();
			_binding.onInactive();
		}

		// Then only, setup the callbacks...

		document.addEventListener(
			impl.event,
			function()
			{
				/*<DEBUG>*/
				console.log('*** Got event: '+impl.event+', current inForeground: '+(inForeground?'YES':'NO')+', new inForeground: '+(impl.inForeground() ? 'YES':'NO'));
				/*</DEBUG>*/

				if(inForeground !== impl.inForeground())
				{
					if((inForeground = !inForeground))
					{
						_binding.onActive();
						_binding.onForeground();
					}
					else
					{
						_binding.onBackground();
						// TODO: figure out if the game loop used by HTTP manager is impacted by the above call.
						// If so, we might want to postpone the onInactive() call for a couple of seconds, to allow requests to terminate
						// (hoping no others would be launched from within the previous delegates)
						_binding.onInactive();
					}
				}
				else
				{
					// This actually shouldn't have happened
					/*<DEBUG>*/
					console.log('*** Inconsistent foreground state change');
					/*</DEBUG>*/
				}
			}
		);
	})();

})();
