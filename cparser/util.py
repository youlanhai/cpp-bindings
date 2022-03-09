# -*- coding: utf-8 -*-
from os import path
import imp
import sys

class ExportException(Exception):
	pass

def _S(msg):
	return str(msg)

def info(msg):
	print("info :", _S(msg))

def error(msg):
	msg = _S(msg)
	print("error:", msg)
	raise ExportException(msg)

def fatal(msg):
	print("fatal:", _S(msg))
	exit(-1)


def write_file(file_path, content):
	with open(file_path, "w", encoding="utf-8", newline="\n") as f:
		f.write(content)

def load_py_config(file_path, alias = None):
	if alias is None:
		alias = file_path

	file_path = path.abspath(file_path)
	parent_path = path.dirname(file_path)

	sys.path.append(parent_path)
	module = None
	try:
		module = imp.load_source(alias, file_path)
	finally:
		sys.path.remove(parent_path)

	return module
