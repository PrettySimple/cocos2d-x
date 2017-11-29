R"(
/* CCUserDefault-emscripten.js */ /* UTF8-Côôkie */

// Using c++ raw string literals: http://www.stroustrup.com/C++11FAQ.html#raw-strings


Module.cocos_UserDefault = {

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE

	_storage		:	(function()
	{
		// Storage availability detection stolen from native's app.js. Comments stripped.
		try
		{
			if(
				!(window.localStorage instanceof Storage) ||
				window.localStorage.getItem('dummy') === {}
			)
				throw null;

			return window.localStorage;
		}
		catch(e)
		{
			try
			{
				if(
					!(window.sessionStorage instanceof Storage) ||
					window.sessionStorage.getItem('dummy') === {}
				)
					throw null;

				return window.sessionStorage;
			}
			catch(e)
			{
				var	memFallback = {};

				return {

					setItem		:	function(key, value)
					{
						memFallback[key] = value;
					},

					getItem		:	function(key)
					{
						if(!memFallback.hasOwnProperty(key))
							return null;

						return memFallback[key];
					},

					removeItem	:	function(key)
					{
						if(memFallback.hasOwnProperty(key))
							delete memFallback[key];
					}
				};
			}
		}

	})(),

	// Since we may share the *storage with other modules, or with emscripten itself, always prefix the keys

	_set			:	function(key, value)
	{
		try
		{
			this._storage.setItem('CCUserDefault::'+key, value);
		}
		catch(e)
		{
		}
	},

	_get			:	function(key)
	{
		try
		{
			return this._storage.getItem('CCUserDefault::'+key);
		}
		catch(e)
		{
			return null;
		}
	},

	_remove			:	function(key)
	{
		try
		{
			this._storage.removeItem('CCUserDefault::'+key);
		}
		catch(e)
		{
		}
	},

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC (cpp)

	setValue		:	function(key_ptr, key_len, value_ptr, value_len)
	{
		this._set(Module.Pointer_stringify(key_ptr, key_len), Module.Pointer_stringify(value_ptr, value_len));
	},

	getValue		:	function(key_ptr, key_len)
	{
		// We return 0 if the key was not found, a pointer to a null-terminated string otherwise
		// (since we assume we're storing null-terminated base64-encoded data)
		var	value = this._get(Module.Pointer_stringify(key_ptr, key_len));

		if(value === null)
			return 0;

		// Since we store base64-encoded strings, the stringToUTF8() output should equal key.length
		// (no multi-byte characters expected), yet always invoke lengthBytesUTF8()...

		var	size = lengthBytesUTF8(value) + 1;
		var	ptr = Module._malloc(size);

		Module.stringToUTF8(value, ptr, size);

		return ptr;
	},

	removeValue		:	function(key_ptr, key_len)
	{
		this._remove(Module.Pointer_stringify(key_ptr, key_len));
	}

};




)"	// End of c++ raw string litteral
