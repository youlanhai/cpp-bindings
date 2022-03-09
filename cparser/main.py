# -*- coding: utf-8 -*-
import sys
import os
import shutil
from argparse import ArgumentParser
from os import path

import util
import export_config
from name_filter import NameFilter

module_path = path.dirname(path.abspath(__file__))
toolset_path = path.join(path.dirname(module_path), "toolset")

def main():
	parser = ArgumentParser(description = "export c++ interface to other language")
	parser.add_argument("-o", "--output", help="output path")
	parser.add_argument("-c", "--config", help="configure file path")
	parser.add_argument("-clang-path", help="clang path")
	parser.add_argument("-t", "--template-path", help="template files path")
	parser.add_argument("-g", "--generator-path", help="generator file path")
	parser.add_argument("-l", "--language", default="lua", help="target language")
	parser.add_argument("--debug", action="store_true", help="parse all symbol for debug")
	parser.add_argument("--clean", action="store_true", help="clean output path")

	option = parser.parse_args()

	try:
		run(option)
	except util.ExportException as e:
		print("export failed:", e)
		exit(-1)

	print("export success")

def run(option):
	if not option.output:
		util.fatal("output path can't be empty")
	if not option.config:
		util.fatal("configuire file path can't be empty")

	if option.config:
		load_configure(option.config)

	if not path.isabs(option.output):
		option.output = path.join(os.getcwd(), option.output)

	compiler_args = get_compiler_args(option)
	clang_path = compiler_args["clang_path"]
	print("clang path:", clang_path)

	from clang import cindex
	if clang_path:
		cindex.Config.set_library_path(clang_path)

	export_config.clang_args.extend(compiler_args["clang_args"])

	generator = None
	if option.generator_path:
		generator = util.load_py_config(option.generator_path)
	else:
		generator_path = "codegen.%s.generator" % option.language
		print("import generator:", generator_path)
		generator = __import__(generator_path, globals(), locals(), ["*"])

	if option.clean and path.exists(option.output):
		shutil.rmtree(option.output)
	
	if not path.exists(option.output):
		os.makedirs(option.output)

	generator_config = {
		"output_path" : option.output,
		"template_path" : option.template_path or path.join(module_path, "codegen", option.language, "templates"),
		"name_filter" : NameFilter(),
	}

	parser = generator.Parser(generator_config)
	parser.export_all = option.debug
	parser.parse()

	if option.debug or export_config.verbose:
		parser.save(path.join(option.output, "test.json"))

	if not option.debug:
		gen = generator.Generator(generator_config)
		gen.all_types = parser.types
		gen.generate(parser.root)


def load_configure(file_path):
	print("load configuire:", file_path)
	config = util.load_py_config(file_path)
	for k, v in config.__dict__.items():
		if not k.startswith("_"):
			setattr(export_config, k, v)
	return

def get_compiler_args(option):
	options = {
		"c++11" : export_config.enable_cxx11,
	}

	if sys.platform == "darwin":
		import util_macosx
		return util_macosx.get_compiler_args(options)

	elif sys.platform == "win32":
		return get_compiler_args_win32(option)

	else:
		util.error("doesn't support this platform:" + sys.platform)

def _find_clang_path_win32():
	clang_path = os.getenv("LLVM_PATH")
	if clang_path:
		system_paths = os.getenv("PATH")
		system_paths = "%s;%s" % (system_paths, clang_path)
		os.putenv("PATH", system_paths)
		return clang_path

	system_paths = os.getenv("PATH").split(';')
	system_paths.insert(0, os.getcwd())
	for search_path in system_paths:
		libclang = path.join(search_path, "libclang.dll")
		if path.exists(libclang):
			return search_path

	util.error("clang path was not found. please install LLVM and set folder 'bin' to system environment")
	return None

def get_compiler_args_win32(option):
	# 只需要安装libclang库就行了，不用再安装llvm了
	clang_path = "" # _find_clang_path_win32()
	return {
		"clang_path" : clang_path,
		"clang_args" : [],
	}


if __name__ == "__main__":
	main()
