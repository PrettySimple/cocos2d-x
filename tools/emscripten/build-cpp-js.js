#!/usr/bin/env node
'use strict';

var	fs = require('fs');
var	uglify = require('uglify-es');

///////////////////////////////////////////////////////////////////////////////////////////
// TOOLS

function	usage()
{
	console.log('');
	console.log('build-cpp-js.js --source <source file (.src.js)> [--[no-]minimize]');
	console.log('');
	console.log('The --[no-]options may be provided multiple times, the last one wins. They all default to \'no\'.');
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
			arguments[i] = new Buffer(arguments[i].toString(), 'utf8');

		size += arguments[i].byteLength;
	}

	if(Buffer.allocUnsafe)
		output = Buffer.allocUnsafe(size);
	else
		output = new Buffer(size);

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


function	minifyJS(content, name)
{
	content = uglify.minify(content.toString());

	if(content.error !== undefined || typeof(content.code) !== 'string')
		throw new Error('Failed minimizing: '+name);

	return new Buffer(content.code, 'utf8');
}


function	build_cpp_js(input_file, output_file, opts)
{
	// C++ raw string literals: http://www.stroustrup.com/C++11FAQ.html#raw-strings
	var	start_delimiter = 'R"***(';
	var	end_delimiter = ')***"';

	var	input_content = fs.readFileSync(input_file);
	var	output_content;

	if(opts.minimize)
		input_content = minifyJS(input_content);

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
	var	opts = {
		minimize:			false
	};

	var	arg, opt;

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

	output_file = input_file.substr(0, input_file.length-input_suffix.length) + output_suffix;

	build_cpp_js(input_file, output_file, opts);

	process.exit(0);

})();


