//////////////////////////////////////////////////////////////////////
/// Desc  LuaWrap
/// Time  2022/03/09
/// Author youlanhai
/// Home   https://github.com/youlanhai/cpp-bindings
//////////////////////////////////////////////////////////////////////
#include "LuaWrap.hpp"
#include "LuaFunction.hpp"
#include "LogTool.hpp"

#include <unordered_map>

NS_LS_BEGIN

static std::unordered_map<std::string, std::string> g_luaType;
static std::unordered_map<std::string, std::string> g_typeCast;

static int gettag;
static int settag;
static int basetag;
static int errortag;
static int errorreporttag;

LS_API void lslua_register_type(const char *cppClassName, const char *luaClassName)
{
    if (cppClassName == nullptr)
    {
        return;
    }

    g_luaType[cppClassName] = luaClassName;
}

LS_API const char* lslua_find_type(const char *cppClassName)
{
    if (cppClassName == nullptr)
    {
        return nullptr;
    }

    auto it = g_luaType.find(cppClassName);
    if (it != g_luaType.end())
    {
        return it->second.c_str();
    }
    return nullptr;
}

LS_API void lslua_error(lua_State *L, LSLuaError *err)
{
    if (err->message == nullptr)
    {
        const char *expectedType = err->expectedType ? err->expectedType : "";
        luaL_error(L, "argument #%d: %s expected, but %s\n", err->index, expectedType, err->message);
    }
    else
    {
        const char *expectedType = err->expectedType ? err->expectedType : "";
        const char *givenType = err->givenType ? err->givenType : luaL_typename(L, err->index);
        luaL_error(L, "argument #%d: %s expected, but '%s' was given\n", err->index, expectedType, givenType);
    }
}

LS_API void lslua_error(lua_State *L, LSLuaError *err, const char *className, const char *funName, int argc, int minArgs)
{
    if (err->index == 0)
    {
        luaL_error(L, "%s:%s has wrong number of arguments: %d, was expecting %d\n", className, funName, argc, minArgs);
    }
    else
    {
        lslua_error(L, err);
    }
}

LS_API int lslua_push_error_fun(lua_State *L)
{
    lua_pushlightuserdata(L, &errortag);
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_isfunction(L, -1))
    {
        return lua_gettop(L);
    }

    lua_pop(L, 1); // pop nil

    lua_getglobal(L, "debug");
    lua_getfield(L, -1, "traceback");
    if (lua_isfunction(L, -1))
    {
        lua_remove(L, -2); // remove 'debug'

        // registry[errortag] = traceback
        lua_pushlightuserdata(L, lslua_errortag());
        lua_pushvalue(L, -2); // push 'traceback'
        lua_rawset(L, LUA_REGISTRYINDEX);

        return lua_gettop(L);
    }
    
    lua_pop(L, 2);
    return 0;
}

static int lslua_default_error_report(lua_State *L)
{
    const char *message = nullptr;
    if (lua_gettop(L) >= 1)
    {
        message = lua_tostring(L, 1);
    }

    luaL_traceback(L, L, message, 2);
    LOG_EXCEPTION("%s", lua_tostring(L, -1));
    return 1;
}

LS_API int lslua_push_error_report(lua_State *L)
{
    lua_pushlightuserdata(L, lslua_errorreporttag());
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_isfunction(L, -1))
    {
        return lua_gettop(L);
    }

    lua_pop(L, 1); // pop nil

    lua_pushcfunction(L, lslua_default_error_report);

    // registry[errortag] = traceback
    lua_pushlightuserdata(L, lslua_errorreporttag());
    lua_pushvalue(L, -2); // push 'traceback'
    lua_rawset(L, LUA_REGISTRYINDEX);
    return lua_gettop(L);
}


LS_API void* lslua_gettag()
{
    return static_cast<void*>(&gettag);
}

LS_API void* lslua_settag()
{
    return static_cast<void*>(&settag);
}

LS_API void* lslua_basetag()
{
    return static_cast<void*>(&basetag);
}

LS_API void* lslua_errortag()
{
    return static_cast<void*>(&errortag);
}

LS_API void* lslua_errorreporttag()
{
    return static_cast<void*>(&errorreporttag);
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

static bool _is_class(lua_State *L, int idx, const char *className)
{
    if (!lua_istable(L, idx))
    {
        return false;
    }

    lua_pushliteral(L, "__cname");
    lua_rawget(L, idx > 0 ? idx : idx - 1);
    bool ret = lslua_isstring(L, -1) && 0 == strcmp(lua_tostring(L, -1), className);
    lua_pop(L, 1);
    return ret;
}

static int lslua_object_tostring(lua_State *L)
{
    // stack: obj
    if (lua_getmetatable(L, 1) != 0)
    {
        lua_pushliteral(L, "__cname");
        lua_rawget(L, -2);
        // stack: obj, metatable, cname

        // remove metatable
        lua_remove(L, -2);

        if (lslua_isstring(L, -1))
        {
            lua_pushfstring(L, "%s: %p", lua_tostring(L, -1), lua_topointer(L, 1));
            return 1;
        }

        lua_pop(L, 1);
    }

    lua_pushfstring(L, "%s: %p", luaL_typename(L, 1), lua_topointer(L, 1));
    return 1;
}

static void bind_base_class(lua_State *L, int idx, const char *base)
{
    if (idx < 0)
    {
        idx = lua_gettop(L) + idx + 1;
    }

    // get or create metatable for class
    if (!lua_getmetatable(L, idx))
    {
        lua_newtable(L);

        lua_pushvalue(L, -1);
        lua_setmetatable(L, idx);
    }

    // stack: class, metatable

    lslua_find_or_create_class(L, base);
    // stack: class, metatable, base

    // metatable[__index] = base
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_rawset(L, -4);

    // metatable[base] = base
    lua_pushliteral(L, "base");
    lua_pushvalue(L, -2);
    lua_rawset(L, -4);

    // class[basetag] = base
    lua_pushlightuserdata(L, &basetag);
    lua_pushvalue(L, -2);
    lua_rawset(L, -5);

    lua_pop(L, 2); // pop metatable and base
}

LS_API bool lslua_find_class(lua_State *L, const char *className)
{
    lua_pushglobaltable(L); // global
    lua_pushstring(L, className); // global, key
    lua_rawget(L, -2); // global, value
    if (_is_class(L, -1, className))
    {
        lua_remove(L, -2); // value
        return true;
    }
    else
    {
        lua_pop(L, 2);
        return false;
    }
}

LS_API void lslua_new_class(lua_State *L, const char *className)
{
    LOG_VERBOSE("new class: %s", className);
    lua_newtable(L);

    // class["__cname"] = className
    lua_pushliteral(L, "__cname");
    lua_pushstring(L, className);
    lua_rawset(L, -3);

    // class["__name"] = className
    lua_pushliteral(L, "__name");
    lua_pushstring(L, className);
    lua_rawset(L, -3);

    // class["__index"] = class
    lua_pushliteral(L, "__index");
    lua_pushcfunction(L, lslua_object_index);
    lua_rawset(L, -3);

    // class["__newindex"] = class
    lua_pushliteral(L, "__newindex");
    lua_pushcfunction(L, lslua_object_newindex);
    lua_rawset(L, -3);

    // class["__gc"] = gc
    lua_pushliteral(L, "__gc");
    lua_pushcfunction(L, lslua_common_gc);
    lua_rawset(L, -3);

    /*lua_pushliteral(L, "__tostring");
    lua_pushcfunction(L, lslua_object_tostring);
    lua_rawset(L, -3);*/
}

LS_API bool lslua_find_or_create_class(lua_State *L, const char *className)
{
    lua_pushglobaltable(L); // global
    lua_pushstring(L, className); // global, key
    lua_rawget(L, -2); // global, value
    if (_is_class(L, -1, className))
    {
        lua_remove(L, -2); // remove global. stack: class
        return true;
    }
    lua_pop(L, 1);

    // stack: global

    lslua_new_class(L, className);

    // global[className] = class
    lua_pushstring(L, className);
    lua_pushvalue(L, -2);
    lua_rawset(L, -4);

    lua_remove(L, -2); // remove global. stack: class
    return true;
}

LS_API bool lslua_is_class(lua_State *L, int idx, const char *module)
{
    lua_pushvalue(L, idx); // duplicate the class
    while (lua_istable(L, -1))
    {
        lua_getfield(L, -1, "__cname"); // push class name
        if (lslua_isstring(L, -1) && strcmp(lua_tostring(L, -1), module) == 0)
        {
            lua_pop(L, 2); // pop __cname, and class
            return true;
        }

        lua_pop(L, 1); // pop class name

        if (!lua_getmetatable(L, -1)) // get super class
        {
            break;
        }
        lua_getfield(L, -1, "base");

        lua_remove(L, -2); // remove metatable
        lua_remove(L, -2); // remove last class
    }

    lua_pop(L, 1);
    return false;
}

LS_API bool lslua_is_class(lua_State *L, const char *subClass, const char *className)
{
    if (!lslua_find_class(L, subClass))
    {
        return false;
    }

    bool ret = lslua_is_class(L, -1, className);
    lua_pop(L, 1);
    return ret;
}

LS_API bool lslua_beginmodule(lua_State *L, const char *name, luaL_Reg *methods)
{
    if (name == nullptr || *name == '\0')
    {
        lua_pushglobaltable(L);
        return true;
    }

    if (!lua_istable(L, -1))
    {
        LOG_ERROR("beginmodule %s: top value is not a table!", name);
        return false;
    }
    //stack: parent

    lua_pushstring(L, name); // parent, key
    lua_rawget(L, -2); // parent, value
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1); // parent

        lua_newtable(L); // parent, module
        lua_pushstring(L, name); // parent, module, name

                                 // module["__name"] = name
        lua_pushliteral(L, "__name");
        lua_pushvalue(L, -2);
        lua_rawset(L, -4);

        // parent[name] = module
        lua_pushvalue(L, -2);
        lua_rawset(L, -4);
    }

    if (methods != nullptr)
    {
        lslua_register_methods(L, methods);
    }
    return true;
}

LS_API void lslua_endmodule(lua_State *L)
{
    lua_pop(L, 1);
}

/*
生成类。

类的结构如下：
class = {
name-x = method-x,
__gc = 对象的通用gc函数,
__index = class,
__cname = 类名称,
}

类本质上是一个对象的原表。
但类的原表是一个新table，注意不是其基类
metatable = {
__index = base,
base = base,
}

*/
LS_API bool lslua_beginclass(lua_State *L, const char *name, const char *fullName, const char *base, lua_CFunction gc)
{
    if (!lua_istable(L, -1))
    {
        LOG_ERROR("beginclass %s: top value is not a table", fullName);
        return false;
    }

    if (fullName == nullptr || *fullName == 0)
    {
        fullName = name;
    }

    lua_pushstring(L, name); // name
    lua_rawget(L, -2); // value
    if (lua_isnil(L, -1))
    {
        lua_pop(L, 1); // stack now empty

        lslua_find_or_create_class(L, fullName); // class

        // parent[name] = class
        lua_pushstring(L, name); // class, name
        lua_pushvalue(L, -2); // class, name, class
        lua_rawset(L, -4); // class
    }

    if (!_is_class(L, -1, fullName))
    {
        lua_pop(L, 1);
        LOG_ERROR("beginclass: symbol with name '%s' is not a valid class", name);
        return false;
    }

    if (base && *base)
    {
        bind_base_class(L, -1, base);
    }

    if (gc)
    {
        lua_pushliteral(L, "__gc");
        lua_pushcfunction(L, gc);
        lua_rawset(L, -3);
    }
    return true;
}

LS_API void lslua_endclass(lua_State *L)
{
    lua_pop(L, 1);
}

LS_API void lslua_register_methods(lua_State *L, luaL_Reg *methods)
{
    luaL_Reg *p = methods;
    while (p->name && p->func)
    {
        lua_pushstring(L, p->name);
        lua_pushcfunction(L, p->func);
        lua_rawset(L, -3);
        ++p;
    }
}

LS_API void lslua_register_new(lua_State *L, lua_CFunction func)
{
    // stack: class
    // get metatable
    if (!lua_getmetatable(L, -1))
    {
        lua_newtable(L);

        lua_pushvalue(L, -1);
        // stack: class metatable, metatable
        lua_setmetatable(L, -3);
    }

    // metatable.__call = func
    lua_pushliteral(L, "__call");
    lua_pushcfunction(L, func);
    lua_rawset(L, -3);

    lua_pop(L, 1); // pop metatable
}

LS_API void lslua_register_function(lua_State *L, const char *name, lua_CFunction func)
{
    lua_pushstring(L, name);
    lua_pushcfunction(L, func);
    lua_rawset(L, -3);
}

LS_API int lslua_object_index(lua_State *L)
{
    // stack: obj, key
    if (lua_getmetatable(L, 1) == 0)
    {
        return 0;
    }

    do
    {
        // stack: obj key class

        lua_pushvalue(L, 2);			    // stack: obj key mt key
        lua_rawget(L, -2);					// stack: obj key mt value

        if (!lua_isnil(L, -1))
        {
            return 1;
        }

        lua_pop(L, 1);
        lua_pushlightuserdata(L, &gettag);
        lua_rawget(L, -2);					//stack: obj key mt tget

        if (lua_istable(L, -1))
        {
            lua_pushvalue(L, 2);			//stack: obj key mt tget key
            lua_rawget(L, -2);           	//stack: obj key mt tget value 

            if (lua_isfunction(L, -1))
            {
                lua_pushvalue(L, 1);
                lua_call(L, 1, 1);
                return 1;
            }
        }

        lua_settop(L, 3);

        // base = class[basetag]
        lua_pushlightuserdata(L, &basetag);
        lua_rawget(L, -2);

        lua_remove(L, -2); // remove last class
    } while (lua_istable(L, -1));

    return 0;
}

LS_API int lslua_object_newindex(lua_State *L)
{
    // stack: obj, key, val
    if (lua_getmetatable(L, 1) == 0)
    {
        return 0;
    }

    do
    {
        // stack: obj key val class

        // sets = class[settag]
        lua_pushlightuserdata(L, &settag);
        lua_rawget(L, -2);					//stack: obj key mt tget
        if (lua_istable(L, -1))
        {
            lua_pushvalue(L, 2);			//stack: obj key mt tget key
            lua_rawget(L, -2);           	//stack: obj key mt tget value 

            if (lua_isfunction(L, -1))
            {
                lua_pushvalue(L, 1);
                lua_pushvalue(L, 3);
                lua_call(L, 2, 1);
                return 0;
            }
        }

        lua_settop(L, 4);

        // base = class[basetag]
        lua_pushlightuserdata(L, &basetag);
        lua_rawget(L, -2);

        lua_remove(L, -2); // remove prev class
    } while (lua_istable(L, -1));

    lua_settop(L, 3);

    if (lua_istable(L, 1))
    {
        lua_pushvalue(L, 2);
        lua_pushvalue(L, 3);
        lua_rawset(L, 1);
    }
    else
    {
        luaL_error(L, "can't set property for object");
    }
    return 0;
}

LS_API void lslua_register_getset(lua_State *L, const char *name, lua_CFunction get, lua_CFunction set)
{
    if (get)
    {
        lua_pushliteral(L, "__index");
        lua_rawget(L, -2);
        if (lua_isfunction(L, -1))
        {
            lua_pop(L, 1);
        }
        else
        {
            lua_pop(L, 1);

            lua_pushliteral(L, "__index");
            lua_pushcfunction(L, lslua_object_index);
            lua_rawset(L, -3);
        }

        lua_pushlightuserdata(L, &gettag);
        lua_rawget(L, -2);
        if (!lua_istable(L, -1))
        {
            lua_pop(L, 1);

            lua_newtable(L);

            // class[gettag] = gets
            lua_pushlightuserdata(L, &gettag);
            lua_pushvalue(L, -2);
            lua_rawset(L, -4);
        }

        // stack: class, gets

        lua_pushstring(L, name);
        lua_pushcfunction(L, get);
        lua_rawset(L, -3);
        lua_pop(L, 1); // pop getter
    }

    if (set)
    {
        lua_pushliteral(L, "__newindex");
        lua_rawget(L, -2);
        if (lua_isfunction(L, -1))
        {
            lua_pop(L, 1);
        }
        else
        {
            lua_pop(L, 1);

            lua_pushliteral(L, "__newindex");
            lua_pushcfunction(L, lslua_object_newindex);
            lua_rawset(L, -3);
        }

        lua_pushlightuserdata(L, &settag);
        lua_rawget(L, -2);
        if (!lua_istable(L, -1))
        {
            lua_pop(L, 1);

            lua_newtable(L);

            // class[settag] = {}
            lua_pushlightuserdata(L, &settag);
            lua_pushvalue(L, -2);
            lua_rawset(L, -4);
        }

        // stack: class, gets

        lua_pushstring(L, name);
        lua_pushcfunction(L, set);
        lua_rawset(L, -3);
        lua_pop(L, 1); // pop getter
    }
}

LS_API void lslua_register_module_getset(lua_State *L, const char *name, lua_CFunction get, lua_CFunction set)
{
    if (0 == lua_getmetatable(L, -1))
    {
        lua_newtable(L);

        lua_pushvalue(L, -1);
        lua_setmetatable(L, -2);
    }

    lslua_register_getset(L, name, get, set);
    lua_pop(L, 1);
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

LS_API bool lslua_to_int64(lua_State *L, int idx, int64_t &v, LSLuaError *err)
{
#if LUA_VERSION_NUM >= 503
    int isnum;
    v = (int64_t)lua_tointegerx(L, idx, &isnum);
    if(isnum)
    {
        return true;
    }
#else
    int type = lua_type(L, idx);
    if(type == LUA_TNUMBER)
    {
        v = (int64_t)lua_tointeger(L, idx);
        return true;
    }
    
    if(type == LUA_TSTRING)
    {
        v = 0;
        const char *p = lua_tostring(L, idx);
        int sign = 1;
        if(*p == '-')
        {
            sign = -1;
            ++p;
        }
        
        while(*p)
        {
            v *= 10;
            v += *p - '0';
            ++p;
        }
        
        v *= sign;
        return true;
    }
#endif

    if(err)
    {
        err->index = idx;
        err->expectedType = "number";
    }
    return false;
}

LS_API bool lslua_to_uint64(lua_State *L, int idx, uint64_t &v, LSLuaError *err)
{
#if LUA_VERSION_NUM >= 503
    int isnum;
    v = (uint64_t)lua_tointegerx(L, idx, &isnum);
    if(isnum)
    {
        return true;
    }
#else
    int type = lua_type(L, idx);
    if(type == LUA_TNUMBER)
    {
        v = (uint64_t)lua_tointeger(L, idx);
        return true;
    }
    
    if(type == LUA_TSTRING)
    {
        v = 0;
        const char *p = lua_tostring(L, idx);
        int sign = 1;
        if(*p == '-')
        {
            sign = -1;
            ++p;
        }
        
        while(*p)
        {
            v *= 10;
            v += *p - '0';
            ++p;
        }
        
        v *= sign;
        return true;
    }
#endif

    if(err)
    {
        err->index = idx;
        err->expectedType = "number";
    }
    return false;
}

LS_API bool lslua_to_number(lua_State *L, int idx, float &v, LSLuaError *err)
{
    if(lua_isnumber(L, idx))
    {
        v = (float)lua_tonumber(L, idx);
        return true;
    }
    if(err)
    {
        err->index = idx;
        err->expectedType = "number";
    }
    return false;
}

LS_API bool lslua_to_number(lua_State *L, int idx, double &v, LSLuaError *err)
{
    if(lua_isnumber(L, idx))
    {
        v = lua_tonumber(L, idx);
        return true;
    }
    if(err)
    {
        err->index = idx;
        err->expectedType = "number";
    }
    return false;
}

LS_API bool lslua_to_string(lua_State *L, int idx, char *&str, LSLuaError *err)
{
    if(lslua_isstring(L, idx))
    {
        str = const_cast<char*>(lua_tostring(L, idx));
        return true;
    }
    if(err)
    {
        err->index = idx;
        err->expectedType = "string";
    }
    return false;
}

LS_API bool lslua_to_string(lua_State *L, int idx, const char *&str, LSLuaError *err)
{
    if(lslua_isstring(L, idx))
    {
        str = lua_tostring(L, idx);
        return true;
    }
    if(err)
    {
        err->index = idx;
        err->expectedType = "string";
    }
    return false;
}

LS_API bool lslua_to_string(lua_State *L, int idx, std::string &str, LSLuaError *err)
{
    if(lslua_isstring(L, idx))
    {
        size_t length;
        const char *p = lua_tolstring(L, idx, &length);
        str.assign(p, length);
        return true;
    }
    
    str.clear();
    if(err)
    {
        err->index = idx;
        err->expectedType = "string";
    }
    return false;
}

LS_API bool lslua_to_pointer(lua_State *L, int idx, void **self, const char *className, LSLuaError *err)
{
    *self = nullptr;
    if(lua_isnil(L, idx))
    {
        if(err)
        {
            err->index = idx;
            err->message = "instance is nil";
        }
        return true;
    }
    
    const char *cname = nullptr;
    if(lua_isuserdata(L, idx) && lua_getmetatable(L, idx)) // push metatable
    {
        if(lslua_is_class(L, -1, className))
        {
            lua_pop(L, 1); // pop metatable
            *self = *(void**)lua_touserdata(L, idx);
            if(*self != nullptr)
            {
                return true;
            }

            if(err)
            {
                err->index = idx;
                err->message = "c++ pointer is NULL.";
            }
        }
        
        lua_getfield(L, -1, "__cname");
        if(lslua_isstring(L, -1))
        {
            cname = lua_tostring(L, -1);
        }
        lua_pop(L, 2); // pop cname and metatable
    }

    if(err)
    {
        if(cname == nullptr)
        {
            cname = luaL_typename(L, idx);
        }
        
        err->index = idx;
        err->expectedType = className;
        err->givenType = cname;
        err->message = "c++ pointer is NULL.";
    }
    return false;
}

LS_API bool lslua_to_function(lua_State *L, int idx, LuaFunction&v, LSLuaError *err)
{
    if(lua_isnil(L, idx))
    {
        v.destroy();
        return true;
    }
    if(lua_isfunction(L, idx))
    {
        lua_pushvalue(L, idx);
        int ret = lua_ref(L, true);
        v.setRef(L, ret);
        return true;
    }
    if(err)
    {
        err->index = idx;
        err->expectedType = "function";
    }
    return false;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////


LS_API int lslua_common_gc(lua_State *L)
{
    if (lua_isuserdata(L, -1))
    {
        LSUserData *u = (LSUserData*)lua_touserdata(L, -1);
        if (u && u->addr)
        {
            if (u->freeMethod)
            {
                u->freeMethod(u->addr);
            }
            u->addr = nullptr;
            u->freeMethod = nullptr;
        }
    }
    return 0;
}

LS_API LSUserData* lslua_new_userdata(lua_State *L, const char *className, size_t extraSize)
{
    LSUserData *p = (LSUserData*)lua_newuserdata(L, sizeof(LSUserData) + extraSize);
    p->addr = nullptr;
    p->freeMethod = nullptr;

    if (className == nullptr || *className == '\0')
    {
        className = "LSUnknown";
    }

    lslua_find_or_create_class(L, className);
    lua_setmetatable(L, -2);
    return p;
}

LS_API LSUserData* lslua_new_userdata(lua_State *L, int classIdx, size_t extraSize)
{
    if (classIdx < 0)
    {
        classIdx = lua_gettop(L) + classIdx + 1;
    }

    LSUserData *p = (LSUserData*)lua_newuserdata(L, sizeof(LSUserData) + extraSize);
    p->addr = nullptr;
    p->freeMethod = nullptr;

    if (lua_istable(L, classIdx))
    {
        lua_pushvalue(L, classIdx);
    }
    else
    {
        lslua_find_or_create_class(L, "LSUnknown");
    }
    lua_setmetatable(L, -2);
    return p;
}


LS_API void lslua_push_int64(lua_State *L, int64_t v)
{
#if LUA_VERSION_NUM >= 503
    lua_pushinteger(L, (lua_Integer)v);
#else
    char buffer[32];
    char *p = buffer + 32;
    uint64_t u;

    if (v <= (int64_t)0x7fffffff && v >= (int64_t)(int32_t)0x80000000)
    {
        lua_pushinteger(L, v);
        return;
    }

    *--p = '\0';
    u = v > 0 ? v : -v;
    while (u != 0)
    {
        *--p = '0' + u % 10;
        u /= 10;
    }
    if (v < 0)
    {
        *--p = '-';
    }

    lua_pushstring(L, p);
#endif
}

LS_API void lslua_push_uint64(lua_State *L, uint64_t v)
{
#if LUA_VERSION_NUM >= 503
    lua_pushinteger(L, (lua_Integer)v);
#else
    char buffer[32];
    char *p = buffer + 32;

    if (v <= (uint64_t)0xffffffff)
    {
        lua_pushinteger(L, v);
        return;
    }

    *--p = '\0';
    while (v != 0)
    {
        *--p = '0' + v % 10;
        v /= 10;
    }

    lua_pushstring(L, p);
#endif
}

LS_API LSUserData* lslua_push_pointer(lua_State* L, void* self, int classIdx, const char* luaClass, const char *cppClass)
{
    LSUserData *p;
    if (classIdx != 0)
    {
        p = lslua_new_userdata(L, classIdx, 0);
    }
    else
    {
        if (cppClass != nullptr && *cppClass != '\0')
        {
            const char* cname = lslua_find_type(cppClass);
            if (cname != nullptr)
            {
                luaClass = cname;
            }
        }
        p = lslua_new_userdata(L, luaClass, 0);
    }
    p->addr = self;
    return p;
}

LS_API LSUserData* lslua_push_pointer(lua_State* L, IScript* self, int classIdx, const char* luaClass, const char *cppClass)
{
    IScript::push_script(L, self);
    return nullptr;
}


LS_API void lslua_push(lua_State* L, const LuaFunction& fun)
{
    if (fun)
    {
        lua_getref(L, fun.getRef());
    }
    else
    {
        lua_pushnil(L);
    }
}

/*
table[1] 记录了链表起始结点
table[2] 记录了已经分配的结点数量。也就是数组的长度
从table[3]开始，记录了对象的引用
*/
static const int index_freelist = 1;
static const int index_size = 2;
static const int index_start = 3;

LS_API int lslua_safe_ref(lua_State* L, int t)
{
    int ref;
    if (lua_isnil(L, -1))
    {
        lua_pop(L, 1);  /* remove from stack */
        return LUA_REFNIL;  /* 'nil' has a unique fixed reference */
    }
    t = lua_absindex(L, t);
    lua_rawgeti(L, t, index_freelist);  /* get first free element */
    ref = (int)lua_tointeger(L, -1);  /* ref = t[freelist] */
    lua_pop(L, 1);  /* remove it from stack */
    if (ref != 0)
    {  /* any free element? */
        lua_rawgeti(L, t, ref);  /* remove it from list */
        lua_rawseti(L, t, index_freelist);  /* (t[freelist] = t[ref]) */
    }
    else  /* no free elements */
    {
        lua_rawgeti(L, t, index_size);
        if (lua_isinteger(L, -1))
        {
            ref = (int)lua_tointeger(L, -1) + 1;
        }
        else
        {
            ref = index_start;
        }
        lua_pop(L, 1);

        lua_pushinteger(L, ref);
        lua_rawseti(L, t, index_size);
    }
    lua_rawseti(L, t, ref);
    return ref;
}

LS_API void lslua_safe_unref(lua_State* L, int t, int ref)
{
    if (ref >= index_start)
    {
        t = lua_absindex(L, t);
        lua_rawgeti(L, t, index_freelist);
        lua_rawseti(L, t, ref);  /* t[ref] = t[freelist] */
        lua_pushinteger(L, ref);
        lua_rawseti(L, t, index_freelist);  /* t[freelist] = ref */
    }
}

NS_LS_END
