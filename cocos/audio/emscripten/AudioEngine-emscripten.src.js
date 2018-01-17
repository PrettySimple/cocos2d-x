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

		// TODO: is createBufferSource necessary?
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

	// path-indexed cache of <AudioBuffer> (mp3 ArrayBuffer decoded by browser)
	var	preloadCache = {};


	// TODO: consider NOT honouring the uncache() requests for sounds that are frequently requested?
	// (that requires implementing a tiny LRU)



	/////////////////////////////////////////////////////////////////////////////////////////////
	// HELPERS

	/*
	function	string2ptr(str)
	{
		var	size_cpp = lengthBytesUTF8(str);
		var	ptr = _malloc(size_cpp + 1);	// null terminator. Free'd by CPP.

		return stringToUTF8(str, ptr, size_cpp + 1);
	}
	*/

	function	preloadCallback(path, success, sync)
	{
		console.log('*** preloadCallback('+path+', '+(success ? 'true' : 'false')+', '+(sync ? 'true' : 'false')+')');

		if(cpp_ptr === null)
			return;

		if(sync === true)
		{
			//binding.preloadCallback(cpp_ptr, string2ptr(path), success ? 1 : 0);
			binding.preloadCallback(cpp_ptr, path, success);
			return;
		}

		setTimeout(
			function()
			{
				if(cpp_ptr === null)
					return;
				//binding.preloadCallback(cpp_ptr, string2ptr(path), success ? 1 : 0);
				binding.preloadCallback(cpp_ptr, path, success);
			},
			1
		);
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
			preloadCache = {};

		},


		/********************************************************************************************************************************************/
		/* PRELOADING/UNCACHING (great naming consistency BTW) */

		preload:	function(path_ptr, path_len)
		{
			console.log('*** preload('+path_ptr+', '+path_len+')');

			// Don't fire the callback synchronously if the file is already preloaded.
			// On the other hand, make sure cpp_ptr is still valid when the callback is actually fired.

			var	filePath = Pointer_stringify(path_ptr, path_len);

			console.log('*** preload(): resolved filePath: '+filePath);

			if(preloadCache.hasOwnProperty(filePath))
				preloadCallback(filePath, true, false);	// false => async
			else
			{
				if(context !== null)
				{
					// Isn't cached yet (yet, there might be a pending cache... if the preload requests aren't dedoubled by Cocos, we might
					// be preloading a same file more than once...).

					var	mp3;

					try
					{
						mp3 = FS.readFile(filePath, { encoding: 'binary' });
					}
					catch(e)
					{
						console.log('*** preload() failed reading from disk: '+filePath);
						preloadCallback(filePath, false, false);	// false => async
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
							//console.log('*** DecodeSuccessCallback');
							
							preloadCache[path_ptr] = buffer;
							preloadCallback(filePath, true, true);	// true => ssync
						},
						// DecodeErrorCallback
						function()
						{
							//console.log('*** DecodeErrorCallback');

							// Assume this is because we don't support mp3...
							// Setting context to null, which will effectively prevent all further action regarding the sound
							context = null;

							preloadCallback(filePath, false, true);	// true => ssync
						}
					);
				}
				else
				{
					console.log('*** context is null');
					preloadCallback(filePath, false, false);	// false => async
				}
			}
		},

		uncache:	function(path_ptr, path_len)
		{
			var	filePath = Pointer_stringify(path_ptr, path_len);

			if(preloadCache.hasOwnProperty(filePath))
				delete preloadCache[filePath];

			// Same comment as in uncacheAll() applies here.
		},

		uncacheAll:	function()
		{
			preloadCache = {};
			// Yet it might get filled by pending preloads... TBH, we don't care, as Cocos should have provided a preload cancel mechanism...
		}

	};

})();

