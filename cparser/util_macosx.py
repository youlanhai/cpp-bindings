# -*- coding: utf-8 -*-
import os
from os import path
import util

def get_compiler_args(options):
	if path.exists("/Library/Developer/CommandLineTools"):
		return get_compiler_args_from_commandline(options)
	elif path.exists("/Applications/Xcode.app"):
		return get_compiler_args_from_xcode(options)
	else:
		util.error("clang path was not found. please install xcode or xcode command line")
		return None

def get_compiler_args_from_xcode(options):
	clang_args = [
		"-nostdinc",
		"-I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1",
		"-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include",
	]

	if options.get("c++11"):
		clang_args.append("-std=c++11")

	clang_include_path = "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang"
	version = find_latest_version(clang_include_path)
	clang_args.append("-I%s/%s/include" % (clang_include_path, version))

	return {
		"clang_path" : "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib",
		"clang_args" : clang_args,
	}

def get_compiler_args_from_commandline(options):
	clang_args = [
		"-nostdinc",
		"-I/Library/Developer/CommandLineTools/usr/include/c++/v1",
		"-I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include",
	]

	if options.get("c++11"):
		clang_args.append("-std=c++11")

	clang_include_path = "/Library/Developer/CommandLineTools/usr/lib/clang"
	version = find_latest_version(clang_include_path)
	clang_args.append("-I%s/%s/include" % (clang_include_path, version))

	return {
		"clang_path" : "/Library/Developer/CommandLineTools/usr/lib",
		"clang_args" : clang_args,
	}

def find_latest_version(file_path):
	files = os.listdir(file_path)
	files.sort(reverse = True)
	return files[0]
