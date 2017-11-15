//
//  CCUserDefault-emscripten.cpp
//  Cococ2d-x
//
//  Created by Vedad KAJTAZ on 15/11/2017.
//
//

#include "platform/CCPlatformConfig.h"
#if(CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)

#include "base/CCUserDefault.h"
#include "base/base64.h"
#include "base/ccUtils.h"
#include <mutex>
#include <stdlib.h>
#include <emscripten.h>


/*

* Other CCUserDefault-<platform>.cpp implementations are located in cocos/base, yet we're storing this file in the much more appropriate cocos/platform folder.

* For some obscure reason, the comments in CCUserDefault.h state:

	> All supported platforms other iOS & Android use xml file to save values.

	However, both ios and android still implement some xml-related code, under the KEEP_COMPATABILITY (sic) #ifdef

	Since we don't have any compatibility to maintain, all xml-related methods shall unconditionally be NOOPs.


* Implementing this is annoying as we cannot add functions to the class.

	All the shared code had to be implemented as static functions (that's what the implementations for other platforms do)


* Choosing the backend

	Candidates:

		1) IndexedDB

			IndexedDB is not available at all on some browsers in private browsing mode:

				https://bugzilla.mozilla.org/show_bug.cgi?id=781982
				(not "fixed" as of 15/11/2017)

			=> Discarded (which also discards emscripten's IDBFS)

		2) localeStorage & sessionStorage

			While localStorage is also unavailable on some (all?) browsers in private browsing mode,
			sessionStorage usually is, and has the advantage to expose the same API as localStorage, making the fallback easy.

			Therefore we'll use sessionStorage as fallback, so that at least everything works in a session lifetime,
			although the data won't persist.

			Unfortunately, there is no native emscripten API for this, we need to code everything by ourselves.

			=> Winner

			Addendum: there is: https://github.com/emscripten-ports/Cocos2d/blob/master/cocos2dx/support/user_default/CCUserDefaultEmscripten.cpp
			but it doesn't fallback to sessionStorage which IMHO is a must. Also, it assumes std::string is utf8 (hence DOMString-serializable) which
			it might not be.

		3) In-memory only non-persistent storage

			=> Discarded as requires nearly the same coding effort as the option #2, while providing no advantage.


* Implementation notes

	* const char *key

		Neither the Cocos documentation nor source code specify anything regarding the "const char *key" argument. According to the code, it is 
		expected to be a zero-terminated string.

		For sake of simplicity, we'll *assume* that it always holds a string that forms valid utf8 sequences. Passing a non-utf8 string will break this
		implementation. It's probably safe, as the implementations using XML have the same constraints regarding utf8.

	* Passing lengths to javascript

		We keep passing length to javascript, despite all pointers being null-terminated string, and Pointer_stringify() knowing how to deal with them.
		This allows us to change serializing format without breaking everything.

	* JS implementation

		Because window.*storage implementations only support storing DOMString-s, and we need to store CPP native data types, including
		binary data (cocos2d::Data), which might not be (unlikely is) valid UTF-16, we'll serialize all data before storing it.

		Therefore, CPP will always provide pre-encoded data that is valid DOMString. Specifically, CPP will always base64-encode/decode everything.

		Also, for compatibility with implementations on other platforms, which serialize every data type into strings, we'll do the same
		and always provide strings to the JS underlying storage.

		This implies a risk of data precision loss of float/double, and that's not the way I would have done it, but it's safer that we
		mimic behavior of other platforms, then exhibit different behavior...


*/

NS_CC_BEGIN


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static functions


static void	setValueForKey(const char *key, const char *value, size_t size)
{
	if(key && value)
	{
		// Okay, we're unconditionally base64-encoding so that the data provided to JS is always DOMString-compatible.
		// We're using cocos' base64 functions (which is what the default CCUserDefault.cpp implementation does, yet only
		// when storing cocos2d::Data)

		char *encodedData = nullptr;
		
		base64Encode(reinterpret_cast<unsigned char *>(const_cast<char *>(value)), size, &encodedData);

		if(!encodedData)
			return;

		EM_ASM_({
			Module.cocos_UserDefault.setValue($0, $1, $2, $3);
		}, key, strlen(key), encodedData, strlen(encodedData));

		free(encodedData);
	}
}

static std::pair<bool, std::string>	// <found, value>
			getValueForKey(const char *key)
{
	uintptr_t	ptr = EM_ASM_INT({
		return Module.cocos_UserDefault.getValue($0, $1);
	}, key, strlen(key));

	if(!ptr)
		return std::make_pair(false, "");

	char	*value = reinterpret_cast<char *>(ptr);

	// Skip useless base64Decode() call if we got an empty string
	if(*value == 0x00)
	{
		free(value);
		return std::make_pair(true, "");
	}

	char	*decodedData = nullptr;
	auto	decodedDataLen = base64Decode(reinterpret_cast<unsigned char *>(value), static_cast<unsigned int>(strlen(value)), reinterpret_cast<unsigned char **>(&decodedData));

	free(value);

	if(decodedData)
	{
		auto	ret = std::make_pair(true, std::string(decodedData, decodedDataLen));

		free(decodedData);
		
		return ret;
	}

	// This is an allocation failure. The default implementation acts as the key wasn't found, so we're doing the same

	return std::make_pair(false, "");
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/destructor and JS implementation (in UserDefault::UserDefault())


UserDefault	*UserDefault::_userDefault = nullptr;
std::string	UserDefault::_filePath = std::string("");
bool		UserDefault::_isFilePathInitialized = false;


UserDefault::UserDefault()
{
	static std::once_flag	run_once;

	// Dump the Javascript that actually does most of the job
	std::call_once(
		run_once,
		[]()
		{
			// I had trouble inlining this code with EM_ASM() (it confuses the compiler), same problem as in HTTPManager_emscripten.cpp...
			const char *code =
				#include "CCUserDefault-emscripten.js"
			;
			emscripten_run_script(code);
		}
	);
}


UserDefault::~UserDefault()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dumb get*() wrappers with default values

bool	UserDefault::getBoolForKey(const char *key)
{
	return getBoolForKey(key, false);
}

int		UserDefault::getIntegerForKey(const char *key)
{
	return getIntegerForKey(key, 0);
}

float	UserDefault::getFloatForKey(const char *key)
{
	return getFloatForKey(key, 0.0f);
}

double	UserDefault::getDoubleForKey(const char *key)
{
	return getDoubleForKey(key, 0.0);
}

std::string	UserDefault::getStringForKey(const char *key)
{
	return getStringForKey(key, "");
}

Data	UserDefault::getDataForKey(const char *key)
{
	return getDataForKey(key, Data::Null);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The actual get*() implementation

bool	UserDefault::getBoolForKey(const char *key, bool defaultValue)
{
	auto	value = getValueForKey(key);

	if(value.first)
		return value.second == "true";

	return defaultValue;
}


int		UserDefault::getIntegerForKey(const char *key, int defaultValue)
{
	auto	value = getValueForKey(key);

	if(value.first)
		return atoi(value.second.c_str());
	
	return defaultValue;
}



float	UserDefault::getFloatForKey(const char *key, float defaultValue)
{
    return static_cast<float>(getDoubleForKey(key, static_cast<double>(defaultValue)));
}


double	UserDefault::getDoubleForKey(const char *key, double defaultValue)
{
	auto	value = getValueForKey(key);

	if(value.first)
		return utils::atof(value.second.c_str());
	
	return defaultValue;
}




std::string	UserDefault::getStringForKey(const char *key, const std::string& defaultValue)
{
	auto	value = getValueForKey(key);

	if(value.first)
		return value.second;

	return defaultValue;
}



Data	UserDefault::getDataForKey(const char *key, const Data& defaultValue)
{
	auto	value = getValueForKey(key);

	if(!value.first)
		return defaultValue;

	if(!value.second.empty())
	{
		Data	ret;

		ret.copy(reinterpret_cast<const unsigned char *>(value.second.data()), value.second.size());

		return ret;
	}
	else
		return Data::Null;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The actual set*(), flush() and deleteValueForKey() implementation


void	UserDefault::setBoolForKey(const char *key, bool value)
{
	if(key)
		setStringForKey(key, value ? "true" : "false");
}

void	UserDefault::setIntegerForKey(const char *key, int value)
{
	if(key)
	{
		// From base/CCUserDefault.cpp ...
		char tmp[50];
		memset(tmp, 0, 50);
		sprintf(tmp, "%d", value);

		setValueForKey(key, tmp, strlen(tmp));
	}
}

void	UserDefault::setFloatForKey(const char *key, float value)
{
	// From CCUserDefault.cpp ...
	setDoubleForKey(key, value);
}

void	UserDefault::setDoubleForKey(const char *key, double value)
{
	if(key)
	{
		// From base/CCUserDefault.cpp ...
		char tmp[50];
		memset(tmp, 0, 50);
		sprintf(tmp, "%f", value);

		setValueForKey(key, tmp, strlen(tmp));
	}

}

void	UserDefault::setStringForKey(const char *key, const std::string& value)
{
	if(key)
		setValueForKey(key, value.data(), value.size());
}

void	UserDefault::setDataForKey(const char *key, const Data& value)
{
	// Note that this will serialize Data::Null as a zero-length string. This is consistent with other implementations.

	if(key)
		setValueForKey(key, reinterpret_cast<const char *>(value.getBytes()), value.getSize());
}

void	UserDefault::flush()
{
	// NOOP in emscripten, all operations are synchronous
}

void	UserDefault::deleteValueForKey(const char *key)
{
	EM_ASM_({
		Module.cocos_UserDefault.removeValue($0, $1);
	}, key, strlen(key));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The rest... most are NOOPs, copied from default or other platforms implementations

UserDefault
		*UserDefault::getInstance()
{
	if(!_userDefault)
		_userDefault = new (std::nothrow) UserDefault();

	return _userDefault;
}

void	UserDefault::destroyInstance()
{
    CC_SAFE_DELETE(_userDefault);
}

// I couldn't find any implementation (nor call) of this function for other platforms (except winrt...)
void	UserDefault::setDelegate(UserDefault *delegate)
{
	if(_userDefault)
		delete _userDefault;

	_userDefault = delegate;
}

UserDefault
		*UserDefault::sharedUserDefault()
{
    return UserDefault::getInstance();
}

void	UserDefault::purgeSharedUserDefault()
{
    UserDefault::destroyInstance();
}



const std::string&	
		UserDefault::getXMLFilePath()
{
	return _filePath;	// always ""
}

bool	UserDefault::isXMLFileExist()
{
	return false;
}

bool	UserDefault::createXMLFile()
{
    return false;
}

void	UserDefault::initXMLFilePath()
{
}


NS_CC_END

#endif	// #if(CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)