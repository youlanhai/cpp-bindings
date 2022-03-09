# -*- coding: utf-8 -*-

to_native = {
	"__fallback"        : "lslua_to_value(L, ${arg_idx}, ${out_value}, \"${scriptname}\", &tolua_err)",
	"__enum"            : "lslua_to_enum(L, ${arg_idx}, ${out_value}, &tolua_err)",
	# lua to native
	"bool"              : "lslua_to_boolean(L, ${arg_idx}, ${out_value}, &tolua_err)",
	# numbers
	"char"              : "lslua_to_number(L, ${arg_idx}, ${out_value}, &tolua_err)",
	"unsigned char"     : "lslua_to_number(L, ${arg_idx}, ${out_value}, &tolua_err)",
	"short"             : "lslua_to_number(L, ${arg_idx}, ${out_value}, &tolua_err)",
	"unsigned short"    : "lslua_to_number(L, ${arg_idx}, ${out_value}, &tolua_err)",
	"int"               : "lslua_to_number(L, ${arg_idx}, ${out_value}, &tolua_err)",
	"unsigned int"      : "lslua_to_number(L, ${arg_idx}, ${out_value}, &tolua_err)",
	"long"              : "lslua_to_number(L, ${arg_idx}, ${out_value}, &tolua_err)",
	"unsigned long"     : "lslua_to_number(L, ${arg_idx}, ${out_value}, &tolua_err)",
	"long long"         : "lslua_to_number(L, ${arg_idx}, ${out_value}, &tolua_err)",
	"unsigned long long": "lslua_to_number(L, ${arg_idx}, ${out_value}, &tolua_err)",
	"float"             : "lslua_to_number(L, ${arg_idx}, ${out_value}, &tolua_err)",
	"double"            : "lslua_to_number(L, ${arg_idx}, ${out_value}, &tolua_err)",
	"size_t"            : "lslua_to_number(L, ${arg_idx}, ${out_value}, &tolua_err)",
	"ssize_t"           : "lslua_to_number(L, ${arg_idx}, ${out_value}, &tolua_err)",
	# string
	"std::string"       : "lslua_to_string(L, ${arg_idx}, ${out_value}, &tolua_err)",
	"char*"             : "lslua_to_string(L, ${arg_idx}, ${out_value}, &tolua_err)",
	# object
	"object"            : "lslua_to_object(L, ${arg_idx}, ${out_value}, \"${scriptname}\", &tolua_err)",
	"lua_State*"        : "(${out_value} = L)",
	"StringRef"     	: "lslua_to_string(L, ${arg_idx}, ${out_value}, &tolua_err)",
	"LuaObject" 	  	: "lslua_to_function(L, ${arg_idx}, ${out_value}, &tolua_err)",
	"LuaFunction"   	: "lslua_to_function(L, ${arg_idx}, ${out_value}, &tolua_err)",
}

from_native = {
	"__fallback"        : "lslua_push_value(L, ${in_value}, \"${scriptname}\")",
	"__enum"            : "lua_pushinteger(L, (lua_Integer)${in_value})",
	# native to lua
	"bool"              : "lua_pushboolean(L, ${in_value})",
	# numbers
	"char"              : "lua_pushinteger(L, (lua_Integer)${in_value})",
	"unsigned char"     : "lua_pushinteger(L, (lua_Integer)${in_value})",
	"short"             : "lua_pushinteger(L, (lua_Integer)${in_value})",
	"unsigned short"    : "lua_pushinteger(L, (lua_Integer)${in_value})",
	"int"               : "lua_pushinteger(L, (lua_Integer)${in_value})",
	"unsigned int"      : "lua_pushinteger(L, (lua_Integer)${in_value})",
	"long"              : "lua_pushinteger(L, (lua_Integer)${in_value})",
	"unsigned long"     : "lua_pushinteger(L, (lua_Integer)${in_value})",
	"long long"         : "lua_pushinteger(L, (lua_Integer)${in_value})",
	"unsigned long long": "lua_pushinteger(L, (lua_Integer)${in_value})",
	"float"             : "lua_pushnumber(L, ${in_value})",
	"double"            : "lua_pushnumber(L, ${in_value})",
	"size_t"            : "lua_pushinteger(L, (lua_Integer)${in_value})",
	"ssize_t"           : "lua_pushinteger(L, (lua_Integer)${in_value})",
	# string
	"std::string"       : "lua_pushlstring(L, ${in_value}.c_str(), ${in_value}.size())",
	"char*"             : "lua_pushstring(L, ${in_value})",
	"object"            : 'lslua_push_object(L, ($type_cast)${in_value}, "${scriptname}")',
	"StringRef"     	: "lua_pushlstring(L, ${in_value}.c_str(), ${in_value}.size())",
	"LuaObject" 		: "lslua_push(L, ${in_value})",
	"LuaFunction" 		: "lslua_push(L, ${in_value})",
}
