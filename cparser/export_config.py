# -*- coding: utf-8 -*-

verbose = True

# 导出名称前缀。用来避免命名冲突
prefix = "test"

# 额外指定的命名空间。可避免全局变量污染全局命名空间
namespace = ""

# 要解析的c++头文件
headers = [
]

# 生成代码需要包含的头文件
include_headers = [
]

# 文件头额外的自定义代码
header_extra_code = ""

# 是否启用c++11
enable_cxx11 = False

# clang编译参数
clang_args = [
	"-DDEBUG",
]

"""包含的类型符号。
命名格式 = 类型名称 | [类型名称, [子类型1命名格式, 子类型2命名格式, ...] ]

数组格式的第一位是类型名称，第二位是子类型数组。

子类型符号可以用数组来表示层层级关系，也可以用`::`来表示。如：
"MyNS::MyClass::getName"与
["MyNS", [
	["MyClass", [
		"getName"
	]],
]]是等价的
"""
includes = [
	["MyNS", [
		["MyClass", [
			"getName",
			"setName",
		]],
		"print",
	]],
	"MyNS::MyClass::getAge",
	"MyNS::MyClass::setAge",
	"simpleFun",
	"TestClass",
]

""" 排除掉的类型符号。
格式与includes相同
"""
excludes = [
	"TestClass::testMethod",
]

""" 重命名规则。
命名格式 = [原始名称, 新名称, [ [子类型1命名格式], [子类型2命名格式], ...] ]

格式与includes相似，只不过每个格式中，第一位是原始名称，第二位是新名称，第三位是子类型的
"""
renames = [
	["MyNS", "my_ns", [
		["MyClass", "my_class", [
			["getName", "get_name"],
		]],
	]],
	["MyNS::MyClass::setName", "set_name"],
]

# 关键词改名。避免与语言的关键词同名了，也可用于全局改名
# 格式:
# 	旧名称 -> 新名称
keyword_renames = {
	# "end" : "end_",
}

abstract_classes = [
	"MyNS::BaseClassA",
	"MyNS::BaseClassB",
]

to_native = {

}

from_native = {
	
}

# 支持c语言格式枚举。命名空间会向上一级空间展开
support_c_enum = False

# 每个类输出一个文件
output_individual = True
