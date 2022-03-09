# -*- coding: utf-8 -*-
from os import path

import igenerator
import cparser
import util
from . import type_mapping
import export_config

from Cheetah.Template import Template
from clang import cindex

Parser = cparser.Parser

def split_scope_name(fullname):
	index = fullname.rfind(".")
	if index > 0:
		return fullname[ : index], fullname[index + 1 : ]
	return "", fullname

class Generator(igenerator.IGenerator):
	def __init__(self, generator_config):
		super(Generator, self).__init__(generator_config)

		type_mapping.to_native.update(export_config.to_native)
		type_mapping.from_native.update(export_config.from_native)

	def get_code_generator(self, type_info):
		tp = type(type_info)
		cls = TYPE_MAP.get(tp)
		if cls is None:
			print("failed find generator for type: ", tp)
			return None
		return cls(self)

	def get_new_name(self, fullname):
		name = self.name_filter.get_new_name(fullname, ".")
		if export_config.namespace:
			if fullname == "":
				return export_config.namespace
				
			if fullname in self.all_types:
				return export_config.namespace + '.' + name
		return name

	def get_script_name(self, type_info):
		return self.get_new_name(type_info.fullname)

	def get_script_short_name(self, type_info):
		fullname = self.get_new_name(type_info.fullname)
		return split_scope_name(fullname)[-1]

	def get_register_name(self, type_info):
		fullname = type_info.fullname
		if fullname:
			fullname = fullname.replace("::", "_").replace("~", "_")
			return "lua_register_%s_%s" % (self.prefix, fullname)
		else:
			return "lua_register_%s" % (self.prefix, )

	def get_register_fun_name(self, type_info):
		fullname = type_info.fullname.replace("::", "_").replace("~", "_")
		return "lua_" + fullname

	def get_register_var_name(self, type_info):
		fullname = type_info.fullname.replace("::", "_").replace("~", "_")
		return "lua_" + fullname


	def get_arg_name(self, arg_info, name):
		arg_type = arg_info.value_type
		if arg_type.is_pointer or arg_type.is_enum or arg_type.is_numeric:
			return name

		to_native = type_mapping.to_native

		type_name = cparser.remove_const_prefix(arg_type.whole_name)
		if type_name in to_native or arg_type.name in to_native:
			return name

		if arg_type.canonical_type:
			type_name = cparser.remove_const_prefix(arg_type.canonical_type.whole_name)
			if type_name in to_native:
				return name

		return "*" + name

	def get_arg_typename(self, arg_info):
		arg_type = arg_info.value_type
		return arg_type.whole_name

	def find_type_conversion_template(self, value_type, conversion_map):
		type_name = cparser.remove_const_prefix(value_type.whole_name)

		keys = [type_name, value_type.name]
		if value_type.canonical_type:
			keys.append(cparser.remove_const_prefix(value_type.canonical_type.whole_name))
		if value_type.is_enum:
			keys.append("__enum")
		elif value_type.is_pointer:
			keys.append("object")
		else:
			keys.append("__fallback")

		template = None
		for key in keys:
			template = conversion_map.get(key)
			if template:
				return template
		return None

	def get_arg_check(self, arg_info, index, name):
		arg_type = arg_info.value_type
		template = self.find_type_conversion_template(arg_type, type_mapping.to_native)

		tpl = Template(template, [{
			"arg_idx" : index,
			"out_value" : name,
			"scriptname" : self.get_new_name(arg_type.whole_name),
		}, export_config])

		return str(tpl)

	def get_ret_push(self, return_type, name):
		arg_type = return_type
		type_name = cparser.remove_const_prefix(arg_type.whole_name)
		template = self.find_type_conversion_template(arg_type, type_mapping.from_native)

		tpl = Template(template, [{
			"in_value" : name,
			"scriptname" : self.get_new_name(arg_type.whole_name),
			"type_cast" : type_name,
		}, export_config])

		return str(tpl)

	def filter_members(self, map):
		keys = list(map.keys())
		keys.sort()

		ret = []
		for k in keys:
			v = map[k]
			if v.is_public:
				ret.append(v)
		return ret

	def filter_functions(self, map):
		keys = list(map.keys())
		keys.sort()

		ret = []
		for k in keys:
			v = map[k]
			if v.is_public and not self.if_function_ignored(v):
				ret.append(v)
		return ret

	def filter_variables(self, map):
		keys = list(map.keys())
		keys.sort()

		ret = []
		for k in keys:
			v = map[k]
			if v.is_public and not self.if_value_type_ignored(v.value_type):
				ret.append(v)
		return ret

	def if_value_type_ignored(self, value_type):
		vt = value_type.canonical_type or value_type
		if vt.kind == cindex.TypeKind.FUNCTIONPROTO or vt.kind == cindex.TypeKind.CONSTANTARRAY:
			return True
		
		if self.name_filter._if_excluded(value_type.whole_name):
			# print "unsupported type:", value_type.whole_name
			return True
		if value_type.canonical_type:
			if self.name_filter._if_excluded(value_type.canonical_type.name):
				# print "unsupported type:", value_type.canonical_type.name
				return True
		return False

	def if_function_argument_ignored(self, fun):
		if fun.return_type and self.if_value_type_ignored(fun.return_type):
			return True

		for arg in fun.arguments:
			if self.if_value_type_ignored(arg.value_type):
				return True
		return False

	def if_function_ignored(self, type_info):
		if type_info.is_method:
			if type_info.is_destructor:
				return True
			if type_info.is_constructor and type_info.parent.is_abstract:
				return True

		overloads = type_info.overloads
		# 剔除参数不支持的函数
		for i in range(len(overloads) - 1, -1, -1):
			m = overloads[i]
			if not m.is_public or self.if_function_argument_ignored(m):
				overloads.pop(i)

		if len(overloads) == 0:
			return True

		return False

class GenBase(igenerator.IProcessor):

	def generate(self, type_info):
		super(GenBase, self).generate(type_info)

		self.init_registration_name(type_info.fullname)
		self.signature_name = self.generator.get_register_name(type_info)

	def init_registration_name(self, name):
		self.registration_fullname = self.generator.get_new_name(name)
		self.registration_scope, self.registration_name = split_scope_name(self.registration_fullname)

class GenScopeBase(GenBase):

	template_name = "namespace.c"

	def generate(self, type_info):
		super(GenScopeBase, self).generate(type_info)

		self.code_tpl = self.generator.load_template(self.template_name)
		self.header_tpl = self.generator.load_template("header.c")
		self.output_individual = export_config.output_individual
		self.contents = []

		self.init_members()
		self.gen_header()
		self.gen_variables()
		self.gen_methods()
		self.gen_tail()

		self.gen_innter_types()
		ret = self.gen_finish()
		return ret

	def init_members(self):
		self.inner_variables = self.generator.filter_variables(self.type_info.variables)
		self.inner_methods = self.generator.filter_functions(self.type_info.methods)
		self.inner_types = self.generator.filter_members(self.type_info.children)

	def gen_header(self):
		if not self.output_individual:
			return
		tpl = Template(self.header_tpl, [self, self.generator, export_config])
		self.contents.append(str(tpl))

	def gen_variables(self):
		for t in self.inner_variables:
			code = self.generator.generate(t)
			if code:
				self.contents.append(code)

		return

	def gen_methods(self):
		for t in self.inner_methods:
			code = self.generator.generate(t)
			if code:
				self.contents.append(code)

		return

	def gen_tail(self):
		tpl = Template(self.code_tpl, [self, self.generator, export_config])
		self.contents.append(str(tpl))

	def gen_finish(self):
		if self.output_individual:
			self.save_to_file()
			return None
		else:
			return "\n".join(self.contents)

	def save_to_file(self):
		file_path = path.join(self.generator.output_path, self.signature_name + ".cpp")
		content = "\n".join(self.contents)
		util.write_file(file_path, content)

	def gen_innter_types(self):
		for t in self.inner_types:
			content = self.generator.generate(t)
			if content:
				self.contents.append(content)

class GenNameSpace(GenScopeBase):

	template_name = "namespace.c"

	def gen_header(self):
		print("gen: ", self.signature_name)
		tpl = Template(self.header_tpl, [self, self.generator, export_config])
		self.contents.append(str(tpl))

	def gen_finish(self):
		self.save_to_file()

class GenClass(GenScopeBase):

	template_name = "class.c"

	def init_members(self):
		super(GenClass, self).init_members()

		bases = self.type_info.bases
		self.base_class = ""
		if len(bases) > 0:
			self.base_class = self.generator.get_script_name(bases[0])

class GenEnum(GenScopeBase):
	template_name = "enum.c"

class GenFunction(GenBase):

	template_name = "function.c"

	def generate(self, type_info):
		super(GenFunction, self).generate(type_info)
		
		self.code_tpl = self.generator.load_template(self.template_name)

		self.signature_name = self.generator.get_register_fun_name(type_info)
		tpl = Template(self.code_tpl, [self, self.generator, export_config])
		return str(tpl)


class GenMethod(GenFunction):

	@property
	def template_name(self):
		return "function.c" if self.type_info.is_static else "method.c"

	def generate(self, type_info):
		self.cpp_class_name = type_info.parent.fullname
		self.lua_class_name = self.generator.get_new_name(self.cpp_class_name)

		return super(GenMethod, self).generate(type_info)


class GenVariable(GenBase):
	template_name = "variable.c"

	def generate(self, type_info):
		super(GenVariable, self).generate(type_info)

		# 常量可以直接设置，不需要写成属性
		if type_info.is_const:
			return
		
		self.code_tpl = self.generator.load_template(self.template_name)

		self.signature_name = self.generator.get_register_var_name(type_info)
		tpl = Template(self.code_tpl, [self, self.generator, export_config])
		return str(tpl)

class GenField(GenVariable):

	@property
	def template_name(self):
		return "variable.c" if self.type_info.is_static else "field.c"

	def generate(self, type_info):
		self.cpp_class_name = type_info.parent.fullname
		self.lua_class_name = self.generator.get_new_name(self.cpp_class_name)

		return super(GenField, self).generate(type_info)

TYPE_MAP = {
	cparser.NameSpaceInfo : GenNameSpace,
	cparser.ClassInfo : GenClass,
	cparser.StructInfo : GenClass,
	cparser.EnumInfo : GenEnum,
	cparser.FunctionInfo : GenFunction,
	cparser.VariableInfo : GenVariable,
	cparser.MethodInfo : GenMethod,
	cparser.FieldInfo : GenField,
	cparser.EnumItemInfo : GenVariable,
}
