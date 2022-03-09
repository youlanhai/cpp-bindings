//////////////////////////////////////////////////////////////////////
/// Desc  lua
/// Time  2022/03/09
/// Author youlanhai
/// Home   https://github.com/youlanhai/cpp-bindings
//////////////////////////////////////////////////////////////////////
#pragma once

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#ifndef lua_ref
#	define lua_ref(L,lock) ((lock) ? luaL_ref(L, LUA_REGISTRYINDEX) : \
      (lua_pushstring(L, "unlocked references are obsolete"), lua_error(L), 0))
#endif

#ifndef lua_unref
#define lua_unref(L,ref)        luaL_unref(L, LUA_REGISTRYINDEX, (ref))
#endif

#ifndef lua_getref
#define lua_getref(L,ref)       lua_rawgeti(L, LUA_REGISTRYINDEX, (ref))
#endif

#if (LUA_VERSION_NUM < 502)

#ifndef lua_pushglobaltable
#define lua_pushglobaltable(L) lua_pushvalue(L, LUA_GLOBALSINDEX)
#endif

#endif
