# -*- coding: utf-8 -*-
from clang import cindex
import json
import re
import export_config
import util
from os import path

CursorKind = cindex.CursorKind
TypeKind = cindex.TypeKind

severities = ['Ignored', 'Note', 'Warning', 'Error', 'Fatal']

def _write_indent(f, n):
	for i in range(n):
		f.write('\t')

def print_error(diagnostics):
	errors = []
	for idx, d in enumerate(diagnostics):
		if d.severity >= cindex.Diagnostic.Error:
			errors.append(d)

	if len(errors) == 0:
		return

	print("====\nErrors in parsing headers:")
	for idx, d in enumerate(errors):
		print("%s. <severity = %s,\n    location = %r,\n    details = %r>" % (
			idx + 1, severities[d.severity], d.location, d.spelling))
	print("====\n")
	raise Exception("Fatal error in parsing headers")


def _dump_dict_members(self, ret, attr, members = None):
	if members is None:
		members = getattr(self, attr, ())
	if len(members) > 0:
		datas = []
		for node in members.values():
			v = {}
			node.dump(v)
			datas.append(v)

		ret[attr] = datas
	return

def _dump_list_members(self, ret, attr, members = None):
	if members is None:
		members = getattr(self, attr, ())
	if len(members) > 0:
		datas = []
		for node in members:
			v = {}
			node.dump(v)
			datas.append(v)

		ret[attr] = datas
	return

def remove_const_prefix(name):
	return re.sub(r"const\s+(.*)", r"\1", name)


default_arg_type_arr = [

# An integer literal.
cindex.CursorKind.INTEGER_LITERAL,

# A floating point number literal.
cindex.CursorKind.FLOATING_LITERAL,

# An imaginary number literal.
cindex.CursorKind.IMAGINARY_LITERAL,

# A string literal.
cindex.CursorKind.STRING_LITERAL,

# A character literal.
cindex.CursorKind.CHARACTER_LITERAL,

# [C++ 2.13.5] C++ Boolean Literal.
cindex.CursorKind.CXX_BOOL_LITERAL_EXPR,

# [C++0x 2.14.7] C++ Pointer Literal.
cindex.CursorKind.CXX_NULL_PTR_LITERAL_EXPR,

cindex.CursorKind.GNU_NULL_EXPR,

# An expression that refers to some value declaration, such as a function,
# varible, or enumerator.
cindex.CursorKind.DECL_REF_EXPR
]


class TypeInfo(object):

	is_enum = False
	is_struct = False
	is_union = False
	is_class = False
	is_function = False
	is_method = False
	is_variable = False
	is_namespace = False
	is_unknown = False
	is_enum_item = False

	def __init__(self, parent = None, name = ""):
		super(TypeInfo, self).__init__()

		self.parent = parent
		self.depth = 0

		self.kind = None
		self.name = ""
		self.displayname = ""
		self.fullname = ""
		self.comment = ""
		self.visibility = cindex.AccessSpecifier.PUBLIC

		self.methods = {}
		self.variables = {}
		# 内部类型/命名空间
		self.children = {}

		self.set_name(name)

	def init_cursor(self, cursor):
		self.cursor = cursor
		self.set_name(cursor.spelling)
		self.kind = cursor.kind
		self.displayname = cursor.displayname
		self.comment = cursor.raw_comment or ""

	def set_name(self, name):
		self.name = name
		self.displayname = name
		if self.parent and self.parent.fullname and name:
			self.fullname = "%s::%s" % (self.parent.fullname, name)
		else:
			self.fullname = name

	def parse(self, cursor, parser):
		pass

	def dump(self, ret):
		ret["name"] = self.name
		ret["displayname"] = self.displayname
		ret["fullname"] = self.fullname
		ret["kind"] = str(self.kind)
		ret["comment"] = self.comment

		_dump_dict_members(self, ret, "variables")
		_dump_dict_members(self, ret, "methods")
		_dump_dict_members(self, ret, "children")

	def add_member(self, member):
		if member.is_function:
			self.add_method(member)
		elif member.is_variable:
			self.add_variable(member)
		else:
			self.add_children(member)

	def add_method(self, member):
		if member.name == "addMoveData":
			print("add_method", member.name, member.min_args, len(member.arguments))

		method = self.methods.setdefault(member.name, member)
		method.add_overload(member)

		for i in range(member.min_args, len(member.arguments)):
			m = member.clone()
			m.arguments = member.arguments[:i]
			member.add_overload(m)

	def add_variable(self, member):
		self.variables[member.name] = member

	def add_children(self, member):
		self.children[member.name] = member

	@property
	def is_public(self):
		return self.visibility == cindex.AccessSpecifier.PUBLIC and "@private" not in self.comment

class NameSpaceInfo(TypeInfo):
	is_namespace = True

	def parse(self, cursor, parser):
		parser.parse_children(cursor, self, self.depth + 1)

class ClassInfo(TypeInfo):
	is_class = True

	@property
	def default_visibility(self):
		return cindex.AccessSpecifier.PRIVATE

	def __init__(self, parent = None):
		super(ClassInfo, self).__init__(parent)
		self.current_visibility = self.default_visibility
		self.bases = []
		self.has_constructor = False

	def parse(self, cursor, parser):
		depth = self.depth + 1

		self.is_abstract = parser.name_filter.if_abstract(self.fullname)

		for cu in cursor.get_children():
			if cu.kind == CursorKind.CXX_ACCESS_SPEC_DECL:
				self.current_visibility = cu.access_specifier
				continue

			if cu.kind == CursorKind.CXX_BASE_SPECIFIER:
				base_name = cu.type.spelling
				base = parser.types.get(base_name)
				if base:
					self.bases.append(base)
				else:
					print("Failed to find base class:", base_name)
				continue

			member = parser.parse_type(cu, self, depth)
			if member:
				# 如果已经存在构造函数，则不会自动添加默认的构造函数
				if member.is_method and member.is_constructor:
					self.has_constructor = True

				member.visibility = self.current_visibility
				if member.is_public:
					self.add_member(member)

		if not self.is_abstract:
			for method in self.methods.values():
				if method.is_pure_virtual:
					self.is_abstract = True
					break

		self.add_default_constructor()
		return

	def dump(self, ret):
		super(ClassInfo, self).dump(ret)

		ret["is_abstract"] = self.is_abstract
		ret["bases"] = [base.fullname for base in self.bases]

	def add_default_constructor(self):
		if self.has_constructor or self.is_abstract or (len(self.variables) == 0 and len(self.methods) == 0):
			return

		print("add default constructor:", self.name, len(self.variables), len(self.methods))

		method = MethodInfo(self, self.name)
		method.is_constructor = True
		self.add_method(method)

class StructInfo(ClassInfo):
	is_struct = True

	@property
	def default_visibility(self):
		return cindex.AccessSpecifier.PUBLIC

class FunctionInfo(TypeInfo):
	is_function = True

	def __init__(self, parent = None, name = ""):
		super(FunctionInfo, self).__init__(parent, name = name)

		# 所有重载的函数。也包括当前函数本身
		self.overloads = []
		self.arguments = []
		self.return_type = None
		self.min_args = 0
		self.is_static = False
		self.is_virtual = False
		self.is_pure_virtual = False
		self.is_override = False
		self.is_constructor = False
		self.is_destructor = False

	def init_cursor(self, cursor):
		super(FunctionInfo, self).init_cursor(cursor)
		self.is_static = cursor.is_static_method()
		self.is_virtual = cursor.is_virtual_method()
		self.is_pure_virtual = cursor.is_pure_virtual_method()

	def clone(self):
		ret = FunctionInfo(self.parent)
		ret.init_cursor(self.cursor)
		ret.return_type = self.return_type
		# ret.arguments = self.arguments
		ret.is_override = self.is_override
		ret.min_args = self.min_args
		return ret

	def add_overload(self, method):
		# return self.overloads.append(method)
		index = 0
		for i, m in enumerate(self.overloads):
			if len(method.arguments) < len(m.arguments):
				index = i
				break
		else:
			index = len(self.overloads)
		self.overloads.insert(index, method)

	def parse(self, cursor, parser):
		default_start = None
		for cu in cursor.get_children():
			if cu.kind == CursorKind.PARM_DECL:
				if default_start is None and FunctionInfo.iterate_param_node(cu):
					default_start = len(self.arguments)

				member = VariableInfo()
				member.init_cursor(cu)
				member.parse(cu, parser)
				self.arguments.append(member)

			elif cu.kind == CursorKind.CXX_OVERRIDE_ATTR:
				self.is_override = True

		self.min_args = len(self.arguments) if default_start is None else default_start

		result = cursor.result_type
		if result and result.kind != TypeKind.VOID:
			self.return_type = VariableType.from_type(result)
		return

	def dump(self, ret):
		super(FunctionInfo, self).dump(ret)

		ret["is_static"] = self.is_static
		ret["is_virtual"] = self.is_virtual
		ret["is_pure_virtual"] = self.is_pure_virtual

		overloads = list(self.overloads)
		try:
			overloads.remove(self)
		except:
			pass

		_dump_list_members(self, ret, "overloads", overloads)
		_dump_list_members(self, ret, "arguments")
		if self.return_type:
			v = {}
			self.return_type.dump(v)
			ret["return"] = v

	# return True if found default argument.
	@staticmethod
	def iterate_param_node(param_node, depth=1):
		for node in param_node.get_children():
			# print(">"*depth+" "+str(node.kind))
			if node.kind in default_arg_type_arr:
				return True

			if FunctionInfo.iterate_param_node(node, depth + 1):
				return True

		return False

class MethodInfo(FunctionInfo):
	is_method = True

	def init_cursor(self, cursor):
		super(MethodInfo, self).init_cursor(cursor)

		self.is_constructor = cursor.kind == CursorKind.CONSTRUCTOR
		self.is_destructor = cursor.kind == CursorKind.DESTRUCTOR

class VariableInfo(TypeInfo):
	is_variable = True

	def parse(self, cursor, parser):
		self.value_type = VariableType.from_type(cursor.type)
		self.is_const = self.value_type.is_const
		self.is_static = False

	def dump(self, ret):
		super(VariableInfo, self).dump(ret)

		v = {}
		self.value_type.dump(v)
		ret["value_type"] = v

class FieldInfo(VariableInfo):
	pass

class EnumInfo(TypeInfo):
	is_enum = True

	def parse(self, cursor, parser):
		self.is_scoped = cursor.is_scoped_enum()
		parser.parse_children(cursor, self, self.depth + 1)

		if not self.is_scoped and export_config.support_c_enum:
			self.parent.variables.update(self.variables)

	def dump(self, ret):
		super(EnumInfo, self).dump(ret)
		ret["is_scoped"] = self.is_scoped

class EnumItemInfo(VariableInfo):
	is_enum_item = True

	def parse(self, cursor, parser):
		super(EnumItemInfo, self).parse(cursor, parser)
		self.is_const = True

class UnkownInfo(TypeInfo):
	is_unknown = True

	def parse(self, cursor, parser):
		parser.parse_children(cursor, self, self.depth + 1)

	def dump(self, ret):
		ret["is_unknown"] = True
		super(UnkownInfo, self).dump(ret)


TYPE_INFO_MAP = {
	CursorKind.CLASS_DECL : ClassInfo,
	CursorKind.STRUCT_DECL : StructInfo,
	CursorKind.ENUM_DECL : EnumInfo,
	CursorKind.NAMESPACE : NameSpaceInfo,
	CursorKind.FUNCTION_DECL : FunctionInfo,
	CursorKind.CXX_METHOD : MethodInfo,
	CursorKind.CONSTRUCTOR : MethodInfo,
	CursorKind.DESTRUCTOR : MethodInfo,
	CursorKind.FIELD_DECL : FieldInfo,
	CursorKind.VAR_DECL : VariableInfo,
	CursorKind.ENUM_CONSTANT_DECL : EnumItemInfo,
}

TYPE_KIND_SET = set([
	CursorKind.CLASS_DECL,
	CursorKind.STRUCT_DECL,
	CursorKind.ENUM_DECL,
	CursorKind.NAMESPACE,
])

class VariableType(object):
	def __init__(self, tp):
		super(VariableType, self).__init__()

		self.kind = tp.kind
		self.is_const = tp.is_const_qualified()
		self.is_pointer = False
		self.is_reference = False
		self.is_pod = tp.is_pod()
		self.is_numeric = False
		self.is_enum = tp.get_canonical().kind == TypeKind.ENUM

		self.whole_name = tp.spelling
		if self.is_const:
			self.whole_name = remove_const_prefix(self.whole_name)
		self.name = self.whole_name.split("::")[-1]

		if re.search("(short|int|double|float|long|size_t)$", self.name) is not None:
			self.is_numeric = True

		self.canonical_type = None
		canonical_type = tp.get_canonical()
		if canonical_type and canonical_type != tp:
			self.canonical_type = VariableType.from_type(canonical_type)

	def dump(self, ret):
		ret["name"] = self.name
		ret["whole_name"] = self.whole_name
		ret["kind"] = str(self.kind)
		ret["const"] = self.is_const
		ret["pointer"] = self.is_pointer
		ret["reference"] = self.is_reference
		ret["pod"] = self.is_pod

		if self.canonical_type:
			v = {}
			self.canonical_type.dump(v)
			ret["canonical_type"] = v
		return

	@classmethod
	def from_type(cls, ntype):
		t = None
		if ntype.kind == TypeKind.POINTER:
			t = cls.from_type(ntype.get_pointee())
			t.is_pointer = True
			t.whole_name += "*"

			if t.canonical_type:
				t.canonical_type.is_pointer = True
				t.canonical_type.whole_name += "*"

		elif ntype.kind == TypeKind.LVALUEREFERENCE:
			t = cls.from_type(ntype.get_pointee())
			t.is_reference = True

		else:
			t = cls(ntype)

		t.origin_name = ntype.spelling
		return t


class Parser(object):

	def __init__(self, generator_config):
		super(Parser, self).__init__()

		self.generator_config = generator_config
		self.types = {}
		self.root = NameSpaceInfo()
		# 导出所有符号，用来测试符号导出情况
		self.export_all = False
		self.name_filter = generator_config["name_filter"]

		self.headers = export_config.headers
		self.clang_args = export_config.clang_args

	def parse(self):
		self.index = cindex.Index.create()

		for header in self.headers:
			print("parse:", header, self.clang_args)
			if not path.exists(header):
				util.error("header file not exist: " + header)

			tu = self.index.parse(header, self.clang_args)

			if len(tu.diagnostics) > 0:
				print_error(tu.diagnostics)

			self.root.parse(tu.cursor, self)

	def save(self, file_path):
		datas = {}
		self.root.dump(datas)

		with open(file_path, "w") as f:
			json.dump(datas, f, indent = 4, ensure_ascii = False)

	def parse_type(self, cursor, parent, depth = 0):
		if depth > 20:
			# print "depth too large:", depth
			return

		kind = cursor.kind
		if not self.export_all and kind == CursorKind.COMPOUND_STMT:
			return

		name = cursor.spelling
		if not name:
			# 匿名的枚举没有名称空间，且clang会把兄弟结点错误的当做子节点在迭代
			parent.parse(cursor, self)
			return

		fullname = name
		if parent and parent.fullname and fullname:
			fullname = "%s::%s" % (parent.fullname, fullname)

		if self.if_excluded(fullname):
			return

		node = self.types.get(fullname)
		if node:
			node.parse(cursor, self)
			return

		cls = TYPE_INFO_MAP.get(kind)
		if not cls:
			if self.export_all:
				cls = UnkownInfo
			else:
				return

		node = cls(parent)
		node.init_cursor(cursor)

		if kind in TYPE_KIND_SET:
			self.types[fullname] = node

		node.parse(cursor, self)
		return node

	def parse_children(self, cursor, parent, depth):
		for cu in cursor.get_children():
			node = self.parse_type(cu, parent, depth)
			if node:
				parent.add_member(node)
		return

	def if_excluded(self, fullname):
		return not self.export_all and self.name_filter.if_excluded(fullname)
