# -*- coding: utf-8 -*-
from os import path

prefix = "sample"

module_path = path.dirname(path.abspath(__file__))
root_path = path.dirname(module_path)
print("root path:", root_path)

headers = [
	module_path + "/sample.hpp",
]

# 代码生成时，需要包含哪些头文件
include_headers = [
	"LuaWrap.hpp",
	"../sample.hpp",
]

# 文件头额外的自定义代码
header_extra_code = "USING_NS_LS"

clang_args = [
	"-x",
	"c++",
	"-I%s/lua" % root_path,
	"-I%s/lslua" % root_path,
	"-I%s/sample" % root_path,
]

includes = [
	["MyNS", [
		"MyClass",
		"counter",
		"version",
		"BaseClassA",
		"Test1::Test2",
		"Test1::Test2::Test3",
		["Test1::Test2", [
			"Test4",
			"Test5",
		]],
		"createMyClass",
		"deleteMyClass",
		"SafeClass",
		"print2",
		#"print3",
		"print4",
		"print5",
		"print6",
	]],
	"MyNS::BaseClassB",
	"MyNS::print",
	"MyNS::Test1::Test6",
	"simpleFun",
	"EAges",
]

excludes = [
	["MyNS", [
		["MyClass", [
			"getAge",
			"setAge",
		]],
	]],
	"EAges::EAges_100",
]

renames = [
	["MyNS", "myns", [
		["MyClass", [
			["getName", "get_name"],
			["setName", "set_name"],
		]],
	]],
	["MyNS::MyClass::getAge", "get_age"],
	["MyNS::MyClass::setAge", "set_age"],
]

abstract_classes = [
	"MyNS::BaseClassA",
	"MyNS::BaseClassB",
]

# 每个类输出一个文件
output_individual = False
