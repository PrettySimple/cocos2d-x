/* AudioEngine-emscripten.src.js */ /* UTF8-Côôkie */

'use strict';


/*

Sample console test

var	ctx = new AudioContext();
var	source;

download('djurdjevdan.mp3').then(
	function(mp3)
	{
		ctx.decodeAudioData(mp3).then(
			function(buffer)
			{
				source = ctx.createBufferSource();
				source.buffer = buffer;
				source.connect(ctx.destination);
				source.start(0);
			}
		);
	}
);


--sound-format mp3


References:

	https://github.com/cocos2d/cocos2d-html5/blob/15704c33421bbd236299b6b965e63eb2a6b7172c/cocos2d/audio/CCAudio.js


*/


/*

NOTES

	* The implementation assumes/expects that AudioEngine (and therefore AudioEngineImpl) is a singleton. It maintains a single pointer to a single CPP instance.

*/


Module.cocos_AudioEngine = (function()
{
	var	binding = Module.AudioEngineImpl;

	// This is a first step in detecting the availability of AudioContext and the actual ability to play mp3.
	// 'context' may get null'd later on, if we happen to realize the browser is actually unable to uncompress mp3.

	var context = (function()
	{
		// This is a first step in detecting the availability of AudioContext and the actual ability to play mp3.
		// Sound.context may get null'd later on, if we happen to realize the browser is actually unable to uncompress mp3.

		var	contextClass = window.AudioContext || window.webkitAudioContext || window.mozAudioContext;

		// TODO: is createBufferSource necessary? Remove requirement if no longer present upon final implementation
		if(!contextClass || !contextClass.prototype.decodeAudioData || !contextClass.prototype.createBufferSource)
			return null;

		// Assuming that mime types supported by HTMLMediaElement also apply to AudioContext implementation
		var	audio = document.createElement('audio');

		function	canEventuallyPlay(mime)
		{
			// https://developer.mozilla.org/en-US/docs/Web/API/HTMLMediaElement/canPlayType
			switch(audio.canPlayType(mime))
			{
				case 'probably':
				case 'maybe':
					return true;
			}

			return false;
		}

		// Even the mime type is inconsisten between browsers...
		// https://stackoverflow.com/questions/10688588/which-mime-type-should-i-use-for-mp3

		if(canEventuallyPlay('audio/mp3') || canEventuallyPlay('audio/mpeg3') || canEventuallyPlay('audio/mpeg'))
			return new contextClass();

		return null;

	})();

	// Pointer to the AudioEngineImpl singleton
	var	cpp_ptr = null;

	// Path-indexed cache of <AudioBuffer> (mp3 ArrayBuffer decoded by browser)
	var	preloadCache = {};




	/////////////////////////////////////////////////////////////////////////////////////////////
	// HELPERS


	function	preloadCallback(path, success)
	{
		console.log('*** preloadCallback('+path+', '+(success ? 'true' : 'false')+')');

		if(cpp_ptr !== null)
			binding.preloadCallback(cpp_ptr, path, success);
	}


	return {

		_construct:	function(ptr)
		{
			console.log('*** _construct('+ptr+')');
			cpp_ptr = ptr;
		},

		_destruct:	function()
		{
			console.log('*** _destruct()');
			// Reinitialize all static storage... (if any?)
			cpp_ptr = null;

			// uncacheAll() will keep track of cancelation requests, so that the result of AudioContext::decodeAudioData() is promptly ignored.
			this.uncacheAll();
		},


		/********************************************************************************************************************************************/
		/* PRELOADING/UNCACHING (great naming consistency BTW) */

		preload:	function(path_ptr, path_len)
		{
			var	filePath = Pointer_stringify(path_ptr, path_len);

			console.log('*** preload('+filePath+')');

			if(preloadCache.hasOwnProperty(filePath))
			{
				// This is not supposed to happen, the caller is breaking the paradigm...
				preloadCallback(filePath, true);
				return;
			}

			if(context !== null)
			{
				// We no longer try to deduplicate nor to detect whether there already is a pending request.
				// It's up to the caller to do so.

				var	mp3;

				try
				{
					mp3 = FS.readFile(filePath, { encoding: 'binary' });
				}
				catch(e)
				{
					console.log('*** preload() failed reading from disk: '+filePath);
					preloadCallback(filePath, false);
					return;
				}

				// The MDN doc: https://developer.mozilla.org/en-US/docs/Web/API/BaseAudioContext/decodeAudioData
				// says there is an "older callback syntax" and a "newer promise-based syntax". I have no idea (I didn't look) how to detect which one
				// is supported, so going for the "older callback syntax" here...

				context.decodeAudioData(
					mp3.buffer,				// <ArrayBuffer>
					// DecodeSuccessCallback
					function(buffer)		// <AudioBuffer>
					{
						preloadCache[filePath] = buffer;
						preloadCallback(filePath, true);
					},
					// DecodeErrorCallback
					function()
					{
						// Assume this is because we don't support mp3 (not because mp3 is corrupt)...
						// Setting context to null, which will effectively prevent all further action regarding the sound
						context = null;

						// This is not strictly required...
						if(preloadCache.hasOwnProperty(filePath))
							delete preloadCache[filePath];

						preloadCallback(filePath, false);
					}
				);
			}
			else
			{
				console.log('*** context is null');
				preloadCallback(filePath, false);
			}
		},

		uncache:	function(path_ptr, path_len)
		{
			var	filePath = Pointer_stringify(path_ptr, path_len);

			console.log('*** uncache('+filePath+')');

			if(preloadCache.hasOwnProperty(filePath))
				delete preloadCache[filePath];
		},


		/********************************************************************************************************************************************/
		/* PLAYING */

		play:	function(path_ptr, path_len, loop, volume)
		{
			var	filePath = Pointer_stringify(path_ptr, path_len);

			console.log('*** play('+filePath+', '+(loop?'true':'false')+', '+volume+')');

			return -1;
		}


	};

})();

