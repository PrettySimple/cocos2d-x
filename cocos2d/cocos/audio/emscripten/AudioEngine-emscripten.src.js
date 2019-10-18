/* AudioEngine-emscripten.src.js */ /* UTF8-Côôkie */

'use strict';


/*

NOTES

	* The implementation assumes/expects that AudioEngine (and therefore AudioEngineImpl) is a singleton. It maintains a single pointer to a single CPP instance.


TODO:

	[+] VM2811:174 [Deprecation] GainNode.gain.value setter smoothing is deprecated and will be removed in M64, around January 2018. Please use setTargetAtTime() instead if smoothing is needed. See https://www.chromestatus.com/features/5287995770929152 for more details.
		=> Nothing to be done.
*/


Module.cocos_AudioEngine = (function()
{
	const INVALID_AUDIO_ID = -1;
	const TIME_UNKNOWN = -1.0;
	const binding = Module.AudioEngineImpl;

	// This is a first step in detecting the availability of AudioContext and the actual ability to play mp3.
	// 'context' may get null'd later on, if we happen to realize the browser is actually unable to uncompress mp3.

	let context = (function()
	{
		// This is a first step in detecting the availability of AudioContext and the actual ability to play mp3.
		// Sound.context may get null'd later on, if we happen to realize the browser is actually unable to uncompress mp3.

		const contextClass = window.AudioContext || window.webkitAudioContext || window.mozAudioContext;

		if(!contextClass || !contextClass.prototype.decodeAudioData || !contextClass.prototype.createBufferSource)
			return null;

		/*
			This fails on chrome (possibly others?) with 'VM303:1 Uncaught TypeError: Illegal invocation'
			We'll check once we instanciated the context...

			// Make sure we have contextTime support
			if(typeof(contextClass.prototype['currentTime']) !== 'number')
				return null;
		*/

		// Assuming that mime types supported by HTMLMediaElement also apply to AudioContext implementation
		const audio = document.createElement('audio');

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
		{
			const context = new contextClass();

			if(typeof(context['currentTime']) === 'number')
				return context;
		}

		return null;

	})();

	// Pointer to the AudioEngineImpl singleton
	let	cpp_ptr = null;

	// Path-indexed cache of <AudioBuffer> (mp3 ArrayBuffer decoded by browser)
	let	preloadCache = {};

	// Id-indexed list of playingSound instances
	let	playingSounds = {};
	let	nextPlayingSoundId = 0;


	/////////////////////////////////////////////////////////////////////////////////////////////
	// Class 'playingSound'

	// Assumes that context is always valid (wrappers must do the check - the context MAY become null)

	function	playingSound(id, path, loop, volume)
	{
		/* jshint validthis: true */

		// <AudioBuffer> must be re-created when pausing, hence this method barely saves the params and invokes _playAt(0)

		if(context === null || !preloadCache.hasOwnProperty(path))
			throw null;

		this.id = id;
		this.path = path;					// Only required by the finish callback...
		this.context = context;				// Both the buffer and context might 'disappear'. Hence, we keep a reference of them so that we don't
		this.buffer = preloadCache[path];	// have to check in every single method where we might need them.
		this.loop = loop;
		this.volume = volume;

		// baseTime is the time, related to audioContext, where the sound starts. At this point, it is equivalent
		// to the start time, but may be later-on altered by pause()s and resume()s (when resuming, it will be the
		// currentTime minus the pauseOffset, eventually modulo duration if looping).
		this.baseTime = context.currentTime;
		this.pauseOffset = null;

		this._playAt(0);
	}

	// It's highly unlikely that we run in a browser that doesn't, yet still feature-detect.
	playingSound.volumeSupport = context !== null ? !!context.createGain : false;

	///////////////////////////////////////////////////////
	// META

	playingSound.prototype.getPath = function()
	{
		return this.path;
	};

	///////////////////////////////////////////////////////
	// PRIVATE

	playingSound.prototype._playAt = function(position)
	{
		this.audioNode = this.context.createBufferSource();
		this.audioNode.loop = this.loop;
		this.audioNode.buffer = this.buffer;

		if(playingSound.volumeSupport)
		{
			// audioNode -> gainNode -> context
			this.gainNode = this.context.createGain();
			this.gainNode.gain.value = this.volume;
			this.audioNode.connect(this.gainNode);
			this.gainNode.connect(this.context.destination);
		}
		else
		{
			// audioNode -> context
			this.gainNode = null;
			this.audioNode.connect(this.context.destination);
		}

		this.audioNode.start(0, position);


		// If not looping, setup an end-of-play callback
		if(!this.loop)
		{
			// We're invoking stop() before the finishCallback(), so that the cleanup is done...
			this.finishTimer = window.setTimeout(() => this._onFinish(), (this.buffer.duration - position) * 1000);
		}
		else
			this.finishTimer = null;
	};


	playingSound.prototype._onFinish = function()
	{
		this.stop();
		finishCallback(this.id);
	};

	///////////////////////////////////////////////////////
	// PUBLIC

	playingSound.prototype.setVolume = function(volume)
	{
		this.volume = volume;

		if(this.gainNode !== null)
			this.gainNode.gain.value = volume;
	};

	playingSound.prototype.setLoop = function(loop)
	{
		void(loop);
		throw 'setLoop() not implemented yet!';
	};


	playingSound.prototype.pause = function()
	{
		if(this.pauseOffset !== null)	// already paused?
			return;

		if(this.finishTimer !== null)
		{
			window.clearTimeout(this.finishTimer);
			this.finishTimer = null;
		}

		// There is actually no way to pause an audioNode. We need to stop and record the position for an eventual resume().

		this.pauseOffset = this.context.currentTime - this.baseTime;

		// Take looping into account
		// Using ms precision for the computation should be more than enough...
		if(this.loop && this.pauseOffset > this.buffer.duration)
			this.pauseOffset = (Math.round(this.pauseOffset * 1000) % Math.min(1, Math.round(this.buffer.duration * 1000))) / 1000;

		// Don't bother altering baseTime, it will be needed-if/computed-when we resume().

		// Stop, disconnect and null everything

		this.audioNode.stop();
		this.audioNode.disconnect();
		this.audioNode = null;

		if(playingSound.volumeSupport)
		{
			this.gainNode.disconnect();
			this.gainNode = null;
		}
	};

	playingSound.prototype.resume = function()
	{
		if(this.pauseOffset === null)	// not paused?
			return;

		// Compute the new baseTime, which is required for subsequent pause() calls...
		this.baseTime = this.context.currentTime - this.pauseOffset;
		this._playAt(this.pauseOffset);
		this.pauseOffset = null;
	};

	playingSound.prototype.stop = function()
	{
		// Make the sound stop, and do lightweight cleanup - no other methods shall be invoked afterwards on this instance

		if(this.finishTimer !== null)
			window.clearTimeout(this.finishTimer);

		this.audioNode.stop();
		this.audioNode.disconnect();

		if(playingSound.volumeSupport)
			this.gainNode.disconnect();
	};


	playingSound.prototype.getDuration = function()
	{
		return this.buffer.duration;
	};


	playingSound.prototype.getCurrentTime = function()
	{
		// Cocos doesn't specify what to return for a looping sound...
		// => We're now "rounding"... That being said, PSAudioEngine::getCurrentTime() is still to be implemented.
		//return this.pauseOffset !== null ? this.pauseOffset : this.context.currentTime - this.baseTime;

		if(this.pauseOffset !== null)
			return this.pauseOffset;

		let	currentTime = this.context.currentTime - this.baseTime;

		// Using ms precision for the computation should be more than enough...
		if(this.loop && currentTime > this.buffer.duration)
			currentTime = (Math.round(currentTime * 1000) % Math.min(1, Math.round(this.buffer.duration * 1000))) / 1000;

		return currentTime;
	};


	/////////////////////////////////////////////////////////////////////////////////////////////
	// HELPERS


	function	preloadCallback(path, success)
	{
		//console.log('*** preloadCallback('+path+', '+(success ? 'true' : 'false')+')');

		if(cpp_ptr !== null)
			binding.preloadCallback(cpp_ptr, path, success);
	}

	function	finishCallback(id)
	{
		//console.log('*** finishCallback('+id+')');

		if(playingSounds.hasOwnProperty('s_'+id))
		{
			const path = playingSounds['s_'+id].getPath();
			delete playingSounds['s_'+id];

			if(cpp_ptr !== null)
				binding.finishCallback(cpp_ptr, id, path);
		}
	}



	return Object.freeze({

		_construct:	function(ptr)
		{
			//console.log('*** _construct('+ptr+')');
			cpp_ptr = ptr;
		},

		_destruct:	function()
		{
			//console.log('*** _destruct()');
			// Reinitialize all static storage... (if any?)
			cpp_ptr = null;

			// Reinit cache and sounds while NOT altering nextPlayingSoundId
			preloadCache = {};
			playingSounds = {};
		},

		/********************************************************************************************************************************************/
		/* PRELOADING/UNCACHING (great naming consistency BTW) */

		preload:	function(path_ptr)
		{
			const filePath = UTF8ToString(path_ptr);

			//console.log('*** preload('+filePath+')');

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

				let	mp3;

				try
				{
					mp3 = FS.readFile(filePath, { encoding: 'binary' });
				}
				catch(e)
				{
					//console.log('*** preload() failed reading from disk: '+filePath);
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
				//console.log('*** context is null');
				preloadCallback(filePath, false);
			}
		},

		uncache:	function(path_ptr)
		{
			const filePath = UTF8ToString(path_ptr);

			//console.log('*** uncache('+filePath+')');

			if(preloadCache.hasOwnProperty(filePath))
				delete preloadCache[filePath];
		},



		/********************************************************************************************************************************************/
		/*<DEBUG>*/

		// Invoke from console as: Module.cocos_AudioEngine.dbg_...(...)

		dbg_dump:	function()
		{
			const ret = { preloaded: [], playing: {} };

			for(let filePath in preloadCache)
			{
				if(preloadCache.hasOwnProperty(filePath))
					ret.preloaded.push(filePath);
			}

			for(let playing in playingSounds)
			{
				if(playingSounds.hasOwnProperty(playing))
				{
					ret.playing[playingSounds[playing].id] = {
						path: playingSounds[playing].path,
						loop: playingSounds[playing].loop,
						volume: playingSounds[playing].volume,
						duration: playingSounds[playing].buffer.duration
					};
				}
			}

			return ret;
		},

		dbg_preload:	function(path)
		{
			const path_ptr = this.dbg_str2ptr(path);

			this.preload(path_ptr.ptr, path_ptr.len);

			_free(path_ptr.ptr);
		},

		dbg_play:		function(path, loop, volume)
		{
			const path_ptr = this.dbg_str2ptr(path);

			this.play(path_ptr.ptr, path_ptr.len, loop, volume);

			_free(path_ptr.ptr);
		},

		dbg_str2ptr:	function(str)
		{
			const size_cpp = lengthBytesUTF8(str);
			const ptr = _malloc(size_cpp + 1);	// null terminator. Free'd by CPP.

			stringToUTF8(str, ptr, size_cpp + 1);

			return { ptr: ptr, len: size_cpp };
		},

		/*</DEBUG>*/


		/********************************************************************************************************************************************/
		/* PLAYING */

		play:	function(path_ptr, loop, volume)
		{
			const filePath = UTF8ToString(path_ptr);

			//console.log('*** play('+filePath+', '+(loop?'true':'false')+', '+volume+')');

			const id = ++nextPlayingSoundId;

			try
			{
				playingSounds['s_'+id] = new playingSound(id, filePath, loop, volume);
				return id;
			}
			catch(e)
			{
				// Bother decrementing nextPlayingSoundId?
				return INVALID_AUDIO_ID;
			}
		},

		setVolume: function(id, volume)
		{
			if(playingSounds.hasOwnProperty('s_'+id))
				playingSounds['s_'+id].setVolume(volume);
		},

		setLoop: function(id, loop)
		{
			if(playingSounds.hasOwnProperty('s_'+id))
				playingSounds['s_'+id].setLoop(loop);
		},

		pause:	function(id)
		{
			if(playingSounds.hasOwnProperty('s_'+id))
				playingSounds['s_'+id].pause();
		},

		resume:	function(id)
		{
			if(playingSounds.hasOwnProperty('s_'+id))
				playingSounds['s_'+id].resume();
		},

		stop:	function(id)
		{
			if(playingSounds.hasOwnProperty('s_'+id))
			{
				playingSounds['s_'+id].stop(id);
				delete playingSounds['s_'+id];
				// NOT invoking finishCallback?
			}
		},

		stopAll:	function()
		{
			// Currently not implemented at PSAudioEngine level...
			throw 'stopAll() not implemented yet!';
		},

		getDuration:	function(id)
		{
			if(playingSounds.hasOwnProperty('s_'+id))
				return playingSounds['s_'+id].getDuration();

			return TIME_UNKNOWN;	// Again, great naming consistency @ Cocos
		},

		getCurrentTime:	function(id)
		{
			if(playingSounds.hasOwnProperty('s_'+id))
				return playingSounds['s_'+id].getCurrentTime();

			return 0;
		},

		setCurrentTime:	function(id, time)
		{
			void(id);
			void(time);
			throw 'setCurrentTime() not implemented yet!';
		}

	});

})();

