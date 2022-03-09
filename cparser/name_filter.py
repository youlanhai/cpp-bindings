# -*- coding: utf-8 -*-
import re
import json
import util
import export_config

def find_child_scope(scope, name):
	child_scope = scope.get(name)
	if child_scope:
		return child_scope

	for pattern, child_scope in scope.items():
		if re.match(pattern, name):
			return child_scope

	return None


class NameFilter(object):
	def __init__(self):
		super(NameFilter, self).__init__()

		# 包含规则。格式：
		# {符号 : {
		#	子符号 : {},
		#   ...
		#   }
		# }
		# 
		self.includes = {}

		# 排除规则。格式：
		# [符号::子符号, ]
		self.excludes = set()

		# 抽象类规则，不生成new方法。格式与excludes相同
		self.abstract_classes = set()

		# 改名规则。格式：
		# {符号: {
		#    "newname" : 新名称,
		#    "children": {子符号规则}
		#   }
		# }
		self.renames = {}

		# 关键词改名。避免与语言的关键词同名了，也可用于全局改名
		self.keyword_renames = export_config.keyword_renames

		for symbol in export_config.includes:
			self._add_include(self.includes, symbol)

		for symbol in export_config.excludes:
			self._expand_symbol(self.excludes, None, symbol)

		for symbol in export_config.abstract_classes:
			self._expand_symbol(self.abstract_classes, None, symbol)

		for symbol in export_config.renames:
			self._add_rename(self.renames, symbol)

		if export_config.verbose:
			print("includes", json.dumps(self.includes, indent = 2))
			print("excludes", json.dumps(list(self.excludes), indent = 2))
			print("abstract_classes", json.dumps(list(self.abstract_classes), indent = 2))
			print("renames", json.dumps(self.renames, indent = 2))
			print("keyword_renames", json.dumps(self.keyword_renames, indent = 2))

	def _add_include(self, scope, symbol):
		name = symbol
		children = None
		if isinstance(symbol, list):
			name, children = symbol

		if "::" in name:
			names = name.split('::')
			name = names[0]

			last_symbol = [names.pop(), children]
			for i in range(len(names) - 1, -1, -1):
				last_symbol = [names[i], [last_symbol]]

			children = last_symbol[1]

		cfg = scope.setdefault(name, {})

		if children:
			for child in children:
				self._add_include(cfg, child)
		return

	def _expand_symbol(self, container, parent_name, symbol):
		is_list = isinstance(symbol, list)
		name = symbol[0] if is_list else symbol

		if parent_name:
			name = parent_name + "::" + name

		if is_list and len(symbol) > 1:
			for child in symbol[1]:
				self._expand_symbol(container, name, child)
		else:
			container.add(name)

	def _if_included(self, fullname):
		names = fullname.split("::")
		# 查找父节点的配置
		scope = self.includes
		for i in range(0, len(names) - 1):
			scope = find_child_scope(scope, names[i])
			if scope is None:
				return False

			# 如果没有子节点，表示包含全部子节点
			if len(scope) == 0:
				return True

		name = names[-1]
		return name in scope

	def _if_excluded(self, fullname):
		# 排除需要精确匹配，只排除叶节点
		if fullname in self.excludes:
			return True
			
		for pattern in self.excludes:
			if re.match(pattern, fullname):
				return True
		return False

	def if_abstract(self, fullname):
		return fullname in self.abstract_classes

	def if_excluded(self, fullname):
		return not self._if_included(fullname) or self._if_excluded(fullname)

	def if_included(self, fullname):
		return self._if_included(fullname) and not self._if_excluded(fullname)

	def _add_rename(self, scope, symbol):
		name = symbol[0]
		newname = None
		children = None
		if isinstance(symbol[1], list):
			children = symbol[1]
		else:
			newname = symbol[1]
			if len(symbol) > 2:
				children = symbol[2]

		if "::" in name:
			names = name.split('::')
			name = names[0]

			last_symbol = [names.pop()]
			if newname:
				last_symbol.append(newname)
				newname = None
			if children:
				last_symbol.append(children)

			for i in range(len(names) - 1, -1, -1):
				last_symbol = [names[i], [last_symbol]]

			children = last_symbol[1]

		cfg = scope.setdefault(name, {})
		if newname:
			cfg["newname"] = newname

		if children:
			children_scope = cfg.setdefault("children", {})
			for child in children:
				self._add_rename(children_scope, child)
		return

	def get_new_name(self, fullname, join_char = "::"):
		newnames = []
		names = fullname.split("::")
		scope = self.renames
		for name in names:
			newname = name

			if scope:
				scope = find_child_scope(scope, name)
				if scope:
					newname = scope.get("newname", name)
					scope = scope.get("children")

			newname = newname.strip('_')
			newname = self.keyword_renames.get(newname, newname)
			newnames.append(newname)

		return join_char.join(newnames)
