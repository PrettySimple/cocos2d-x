#!/usr/bin/env node
'use strict';

var	fs = require('fs');
var	jshint = require('jshint').JSHINT;
var	uglify = require('uglify-es');

// Uglify documentation: https://github.com/mishoo/UglifyJS2/


///////////////////////////////////////////////////////////////////////////////////////////
// TOOLS

function	usage()
{
	console.log('');
	console.log('build-cpp-js.js --source <source file (.src.js)>');
	console.log('                [--[no-]debug] [--[no-]lint] [--[no-]minimize] [--[no-]iife]');
	console.log('                (--lint-define-ro <object>)* (--lint-define-rw <object>)*');
	console.log('');
	console.log('The --[no-]options may be provided multiple times, the last one wins. They all default to \'no\'.');
	console.log('The --lint-define-ro and --lint-define-rw options are both cumulative.');
	console.log('Note: "iife" stands for "Immediately Invoked Function Expression".');
	console.log('');
	process.exit(2);
}


function	fileExists(filename)
{
	var	exists;

	try
	{
		var stat = fs.statSync(filename);

		if(!stat.isFile())
			throw new Error('Exists but is not a file: '+filename);

		return true;
	}
	catch(e)
	{
		if(e.code !== 'ENOENT')
			throw e;

		return false;
	}
}

function	concatBuffers()
{
	var	output, size = 0, offset = 0;

	for(var i = 0; i < arguments.length; ++i)
	{
		if(!(arguments[i] instanceof Buffer))
			arguments[i] = Buffer.from(arguments[i].toString(), 'utf8');

		size += arguments[i].byteLength;
	}

	output = Buffer.allocUnsafe(size);

	for(var i = 0; i < arguments.length; ++i)
	{
		// Buffer::copy(target[, targetStart[, sourceStart[, sourceEnd]]])
		arguments[i].copy(output, offset);
		offset += arguments[i].byteLength;
	}

	return output;
}

///////////////////////////////////////////////////////////////////////////////////////////
// BUILD


function	minifyJS(content, name, opts)
{
	content = uglify.minify(content.toString(), opts.iife ? { compress: { negate_iife: false } } : {});

	if(content.error !== undefined || typeof(content.code) !== 'string')
		throw new Error('Failed minimizing: '+name+': '+content.error);

	return Buffer.from(content.code, 'utf8');
}


function	stripDebug(content)
{
	var	start_tag = '/*<DEBUG>*/', end_tag = '/*</DEBUG>*/';
	var	pos_start, pos_end = 0;

	while((pos_start = content.indexOf(start_tag, pos_end)) !== -1)
	{
		if((pos_end = content.indexOf(end_tag, pos_start + start_tag.length)) === -1)
			throw new Error('Syntax error while parsing '+start_tag+' blocks (could not match closing '+end_tag+')');

		content = concatBuffers(
			content.slice(0, pos_start),
			content.slice(pos_end += end_tag.length)
		);
	}

	return content;
}



function	build_cpp_js(input_file, output_file, opts, lint_define_ro, lint_define_rw)
{
	// C++ raw string literals: http://www.stroustrup.com/C++11FAQ.html#raw-strings
	var	start_delimiter = 'R"***(';
	var	end_delimiter = ')***"';

	var	input_content = fs.readFileSync(input_file);
	var	output_content;

	if(!opts.debug)
		input_content = stripDebug(input_content);

	if(opts.lint)
	{
		var	lint_ignore = [];	// JSHINT error codes to ignore (eg. W069). Beware that they still account against maxerr.

		var	lint_options = {
			esversion: 6,
			sub: true,				// 'W069' - although documented as deprecated, there is currently no alternative to this option
			freeze: true,
			futurehostile: true,
			noarg: true,
			strict: 'global',
			unused: true,
			browser: true,
			maxerr: 999999
		};

		var	lint_defines = {
			// Hopefully complete list of Emscripten-provided globals we used till now (hence, to be completed as we use more features)
			// Developers need to explicitly mark other 'globals' to avoid 'undefined' errors... (or use the --lint-define-* options)
			HEAPU8: true,
			FS: false,
			Module: false,
			UTF8ToString: false,
			lengthBytesUTF8: false,
			stringToUTF8: false,
			stringToUTF8Array: false,
			allocateUTF8: false,
			_malloc: false,
			_free: false
		};

		var	lint_errors = [];

		// Add user-provided defines to lint_defines...
		lint_define_ro.forEach(function(glob) { lint_defines[glob] = false; });
		lint_define_rw.forEach(function(glob) { lint_defines[glob] = true; });


		jshint(
			input_content.toString(),
			lint_options,
			lint_defines
		);

		jshint.errors.forEach(
			function(error)
			{
				if(lint_ignore.findIndex(function(ignore) { return ignore === error.code; }) === -1)
					lint_errors.push('\tError #'+error.code+' at line '+error.line+': '+error.reason);
			}
		);

		if(jshint.errors.length)
			throw new Error('--lint reported '+jshint.errors.length+' error(s) in '+input_file+':\n'+lint_errors.join('\n')+'\n');
	}

	if(opts.minimize)
		input_content = minifyJS(input_content, input_file, opts);

	output_content = concatBuffers(
		start_delimiter,
		input_content,
		end_delimiter
	);

	// Check the existance of output file, eventually read it and match the contents with output_content
	// If identical, we won't overwrite to keep the mtime unchanged, so that [c]make doesn't trigger a compile

	if(!fileExists(output_file) || !output_content.equals(fs.readFileSync(output_file)))
		fs.writeFileSync(output_file, output_content);
}




///////////////////////////////////////////////////////////////////////////////////////////
// MAIN

(function()
{
	var	args = process.argv;
	var	input_file = null, output_file;
	var	input_suffix = '.src.js', output_suffix = '.cpp.js';
	var	lint_define_ro = [], lint_define_rw = [];
	var	opts = {
		debug:				false,
		lint:				false,
		minimize:			false,
		iife:				false
	};

	var	arg;

	args.shift();
	args.shift();

	while(args.length)
	{
		switch(arg = args.shift())
		{
			case '--source':
				if(!args.length || input_file !== null)
					usage();
				input_file = args.shift();
				break;

			case '--lint-define-ro':
				if(!args.length)
					usage();
				lint_define_ro.push(args.shift());
				break;

			case '--lint-define-rw':
				if(!args.length)
					usage();
				lint_define_rw.push(args.shift());
				break;

			default:
				{
					var	opt, val;

					if(arg.substr(0, 2) === '--')
					{
						if(arg.substr(2, 3) === 'no-')
						{
							opt = arg.substr(5);
							val = false;
						}
						else
						{
							opt = arg.substr(2);
							val = true;
						}

						opt = opt.replace(/\-/g, '_');

						if(!opts.hasOwnProperty(opt))
							usage();

						opts[opt] = val;
					}
					else
						usage();
				}
		}
	}

	if(input_file === null)
		usage();

	if(input_file.substr(-input_suffix.length) !== input_suffix)
		throw new Error('Source file suffix must be: '+input_suffix);

	if((lint_define_ro.length || lint_define_rw.length) && opts.lint === false)
		throw new Error('Incompatible option: --lint-define-* without --lint');

	output_file = input_file.substr(0, input_file.length-input_suffix.length) + output_suffix;

	build_cpp_js(input_file, output_file, opts, lint_define_ro, lint_define_rw);

	process.exit(0);

})();


