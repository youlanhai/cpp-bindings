//////////////////////////////////////////////////////////////////////
/// Desc  LuaWrap
/// Time  2022/03/09
/// Author youlanhai
/// Home   https://github.com/youlanhai/cpp-bindings
//////////////////////////////////////////////////////////////////////
#pragma once
#include "IScript.hpp"
#include <typeinfo>
#include <string>

// predefine
NS_LS_BEGIN
class LuaFunction;

struct LSLuaError
{
    int index = 0;
    const char* expectedType = nullptr;
    const char* givenType = nullptr;
    const char* message = nullptr;
};

/// lua userdata 数据的封装类。
struct LSUserData
{
    typedef void(*FreeMethod)(void*);

    /// 指向真实数据地址。addr变量必须放在起始地址，这样可以通过 *((void**)lua_touserdata(L, -1))，取到的内容就是addr指向的内容
    void *addr;
    FreeMethod freeMethod;
};

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

/** 注册c++类型到lua类型的映射关系 */
LS_API void lslua_register_type(const char *cppClassName, const char *luaClassName);

/** 查找c++类型对应的lua类型
 *  @return 查找成功返回lua类型名称，否则返回NULL
 */
LS_API const char* lslua_find_type(const char *cppClassName);

LS_API void lslua_error(lua_State *L, LSLuaError *err);

LS_API void lslua_error(lua_State *L, LSLuaError *err, const char *className, const char *funName, int argc, int minArgs);

/** 处理错误信息。通常是debug.traceback */
LS_API int lslua_push_error_fun(lua_State *L);

/** 处理并上报异常信息 */
LS_API int lslua_push_error_report(lua_State *L);


LS_API void* lslua_gettag();
LS_API void* lslua_settag();
LS_API void* lslua_basetag();
LS_API void* lslua_errortag();
LS_API void* lslua_errorreporttag();

//////////////////////////////////////////////////////////////////////
// 类/模块注册相关
//////////////////////////////////////////////////////////////////////

LS_API bool lslua_beginmodule(lua_State *L, const char *name, luaL_Reg *methods = nullptr);
LS_API void lslua_endmodule(lua_State *L);

/** 注册class到模块，模块必须位于栈顶。
*  @param  name    类型短名称。模块下的名称
*  @param  fullName 类型全名称。全局命名空间下的名称
*  @param  gc      自定义gc方法，一般不需要设置。如果未设置，会使用默认的gc方法
*  @param  base    基类全名称
*/
LS_API bool lslua_beginclass(lua_State *L, const char *name, const char *fullName = nullptr, const char *base = nullptr, lua_CFunction gc = nullptr);
LS_API void lslua_endclass(lua_State *L);

/** 给栈顶元素批量注册方法 */
LS_API void lslua_register_methods(lua_State *L, luaL_Reg *methods);
/** 注册构造函数 */
LS_API void lslua_register_new(lua_State *L, lua_CFunction func);
/** 注册方法 */
LS_API void lslua_register_function(lua_State *L, const char *name, lua_CFunction func);
/** 给类注册get和set属性 */
LS_API void lslua_register_getset(lua_State *L, const char *name, lua_CFunction get, lua_CFunction set);
/** 给模块注册get和set属性。可以通过模块来访问c++的变量。*/
LS_API void lslua_register_module_getset(lua_State *L, const char *name, lua_CFunction get, lua_CFunction set);

template <typename T>
void lslua_register_var(lua_State *L, const char *field, const T& value);

template <typename T>
void lslua_register_var(lua_State *L, const char *field, T *self, const char *className)
{
    lua_pushstring(L, field);
    lslua_push_object(L, self, className);
    lua_rawset(L, -3);
}

//////////////////////////////////////////////////////////////////////
/// 类相关
//////////////////////////////////////////////////////////////////////

/// 在全局命名空间中查找类。如果找到该类，则将其放在栈顶，并返回true；否则，什么也不做，并返回false。
LS_API bool lslua_find_class(lua_State *L, const char *className);
/// 在全局命名空间中查找类。如果没找到，则新建一个类。
LS_API bool lslua_find_or_create_class(lua_State *L, const char *className);

/// 判断类是否是指定类型或其派生类。
LS_API bool lslua_is_class(lua_State *L, int idx, const char *className);
LS_API bool lslua_is_class(lua_State *L, const char *subClass, const char *className);

/** 通用对象属性获取方法。*/
LS_API int lslua_object_index(lua_State *L);
/** 通用对象属性设置方法。*/
LS_API int lslua_object_newindex(lua_State *L);

//////////////////////////////////////////////////////////////////////
/// lua to c++
//////////////////////////////////////////////////////////////////////

/** 判断值是否真的是字符串。
 *  注意与lua接口的差异：
 *      lua_isstring会把数字也当做字符串来考虑。
 *      lua_tostring会把数字转换成字符串放到栈顶上。
 */
inline bool lslua_isstring(lua_State* L, int idx)
{
    return lua_type(L, idx) == LUA_TSTRING;
}

inline bool lslua_toboolean(lua_State *L, int idx)
{
    return 0 != lua_toboolean(L, idx);
}

/// 转换为bool类型，总是会成功。只有nil和false是false，其余都是true
inline bool lslua_to_boolean(lua_State *L, int idx, bool &v, LSLuaError *)
{
    v = 0 != lua_toboolean(L, idx);
    return true;
}

LS_API bool lslua_to_int64(lua_State *L, int idx, int64_t &v, LSLuaError *err);
LS_API bool lslua_to_uint64(lua_State *L, int idx, uint64_t &v, LSLuaError *err);

inline bool lslua_to_number(lua_State *L, int idx, int64_t &v, LSLuaError *err)
{
    return lslua_to_int64(L, idx, v, err);
}

inline bool lslua_to_number(lua_State *L, int idx, uint64_t &v, LSLuaError *err)
{
    return lslua_to_uint64(L, idx, v, err);
}

template <typename T>
bool lslua_to_number(lua_State *L, int idx, T &v, LSLuaError *err)
{
    if(lua_isnumber(L, idx))
    {
        v = (T)lua_tointeger(L, idx);
        return true;
    }
    if(err)
    {
        err->index = idx;
        err->expectedType = "number";
    }
    return false;
}

LS_API bool lslua_to_number(lua_State *L, int idx, float &v, LSLuaError *err);

LS_API bool lslua_to_number(lua_State *L, int idx, double &v, LSLuaError *err);

template <typename T>
bool lslua_to_enum(lua_State *L, int idx, T &v, LSLuaError *err)
{
    if(lua_isnumber(L, idx))
    {
        v = (T)lua_tointeger(L, idx);
        return true;
    }
    if(err)
    {
        err->index = idx;
        err->expectedType = "number";
    }
    return false;
}

LS_API bool lslua_to_string(lua_State *L, int idx, char *&str, LSLuaError *err);
LS_API bool lslua_to_string(lua_State *L, int idx, const char *&str, LSLuaError *err);
LS_API bool lslua_to_string(lua_State *L, int idx, std::string &str, LSLuaError *err);

/** 将对象按给定类型解析为指针。注意：返回的指针可能为空。
* 匹配成功是指，className是当前对象的类或基类。如果对象为空，也表示匹配成功。
* @return 如果类型匹配，则返回true；否则返回false。
*/
LS_API bool lslua_to_pointer(lua_State *L, int idx, void **self, const char *className, LSLuaError *err);

/** 解析指针类型参数。注意：返回的指针可能为空。
* @return 如果类型匹配，则返回true；否则返回false。
*/
template <typename T>
bool lslua_to_object(lua_State *L, int idx, T *&self, const char *className, LSLuaError *err)
{
    return lslua_to_pointer(L, idx, (void**)&self, className, err);
}

/* 解析值类型参数。lua传递过来的数据必然都是分配在堆上的，因此提取参数用指针变量即可。
* 如果需要存贮值，自己做一份拷贝，不要记录指针。
* @return 如果类型匹配，且对象不为空，则返回true；否则返回false。
*/
template <typename T>
bool lslua_to_value(lua_State *L, int idx, T *&self, const char *className, LSLuaError *err)
{
    return lslua_to_pointer(L, idx, (void**)&self, className, err) && self != nullptr;
}

LS_API bool lslua_to_function(lua_State *L, int idx, LuaFunction&v, LSLuaError *err);

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

template <typename T>
void lslua_free_managed_data(void *v)
{
    T *p = (T*)v;
    p->~T();
}

template <typename T>
void lslua_free_unmanaged_data(void *v)
{
    T *p = (T*)v;
    delete p;
}

/// 通用gc方法。调用LSUserData的freeMethod方法，来清理内存。
LS_API int lslua_common_gc(lua_State *L);

/** 在栈顶创建lua userdata，返回数据指针。
* @param className    lua类名称。如果类不存在，则自动创建一个空类；如果参数为空，则创建一个LSUnknown类
* @param extraSize    额外指定的数据空间大小，用来存放自定义数据
* @return 该方法始终会创建成功，并返回LSUserData指针。
*/
LS_API LSUserData* lslua_new_userdata(lua_State *L, const char *className, size_t extraSize = 0);

/** 在栈顶创建lua userdata，返回数据指针。
* @param className    lua类名称。如果类不存在，则自动创建一个空类；如果参数为空，则创建一个LSUnknown类
* @param extraSize    额外指定的数据空间大小，用来存放自定义数据
* @return 该方法始终会创建成功，并返回LSUserData指针。
*/
LS_API LSUserData* lslua_new_userdata(lua_State *L, int classIdx, size_t extraSize = 0);

/** 实例化c++对象，并放到栈顶，常用与lua new方法中使用。
* 注意：外部需要调用 placement new 来构造对象
*/
template <typename T>
void* lslua_managed_new(lua_State *L, int classIdx)
{
    LSUserData* p = lslua_new_userdata(L, classIdx, sizeof(T));
    p->addr = p + 1;
    p->freeMethod = (LSUserData::FreeMethod)lslua_free_managed_data<T>;
    return p->addr;
}

#define LSLUA_MANAGED_NEW(L, CLASS, classIdx, ...) \
    new (lslua_managed_new<CLASS>(L, classIdx))CLASS(__VA_ARGS__)

template <typename T>
LSUserData* lslua_push_value(lua_State *L, const T &self, const char *className)
{
    const char * cname = lslua_find_type(typeid(self).name());
    if (cname != nullptr)
    {
        className = cname;
    }

    // 将数据放在LSUserData描述头后面
    LSUserData *p = lslua_new_userdata(L, className, sizeof(T));
    p->addr = p + 1;
    p->freeMethod = (LSUserData::FreeMethod)lslua_free_managed_data<T>;
    // placement new
    new(p->addr)T(self);
    return p;
}

template <typename T>
void lslua_push_enum(lua_State *L, T v)
{
    lua_pushinteger(L, (lua_Integer)v);
}

LS_API void lslua_push_int64(lua_State *L, int64_t v);

LS_API void lslua_push_uint64(lua_State *L, uint64_t v);


////////////////////////////////////////////////////////////////////////////////
// push系列函数，不能抛出异常
////////////////////////////////////////////////////////////////////////////////

/** 重载此方法，对自定义类型执行内存管理或者绑定gc的方法 */
inline void lslua_post_push(LSUserData *p, void *self) {}

/** 如果构造函数是私有的，重载此方法定义自己的freeMethod */
template <typename T>
void lslua_bind_free_method(LSUserData *p, T *self)
{
    if (p != nullptr && p->freeMethod == nullptr)
    {
        p->freeMethod = (LSUserData::FreeMethod)lslua_free_unmanaged_data<T>;
    }
}

LS_API LSUserData* lslua_push_pointer(lua_State* L, void* self, int classIdx, const char* luaClass, const char *cppClass);
LS_API LSUserData* lslua_push_pointer(lua_State* L, IScript* self, int classIdx, const char* luaClass, const char *cppClass);

template <typename T>
LSUserData* lslua_push_object(lua_State* L, T *self, int classIdx)
{
    if (self == nullptr)
    {
        lua_pushnil(L);
        return nullptr;
    }

    LSUserData *p = lslua_push_pointer(L, self, classIdx, nullptr, nullptr);
    lslua_post_push(p, self);
    return p;
}

template <typename T>
LSUserData* lslua_push_object(lua_State* L, T *self, const char *luaClass)
{
    if (self == nullptr)
    {
        lua_pushnil(L);
        return nullptr;
    }

    LSUserData *p = lslua_push_pointer(L, self, 0, luaClass, typeid(*self).name());
    lslua_post_push(p, self);
    return p;
}

inline void lslua_push(lua_State* L, IScript* v) { IScript::push_script(L, v); }
inline void lslua_push(lua_State* L, bool v) { lua_pushboolean(L, v); }
inline void lslua_push(lua_State* L, int8_t v) { lua_pushinteger(L, (lua_Integer)v); }
inline void lslua_push(lua_State* L, uint8_t v) { lua_pushinteger(L, (lua_Integer)v); }
inline void lslua_push(lua_State* L, int16_t v) { lua_pushinteger(L, (lua_Integer)v); }
inline void lslua_push(lua_State* L, uint16_t v) { lua_pushinteger(L, (lua_Integer)v); }
inline void lslua_push(lua_State* L, int32_t v) { lua_pushinteger(L, (lua_Integer)v); }
inline void lslua_push(lua_State* L, uint32_t v) { lua_pushinteger(L, (lua_Integer)v); }
inline void lslua_push(lua_State* L, int64_t v) { lslua_push_int64(L, v); }
inline void lslua_push(lua_State* L, uint64_t v) { lslua_push_uint64(L, v); }
inline void lslua_push(lua_State* L, float v) { lua_pushnumber(L, (lua_Number)v); }
inline void lslua_push(lua_State* L, double v) { lua_pushnumber(L, (lua_Number)v); }

inline void lslua_push(lua_State* L, const char* v) { lua_pushstring(L, v); }
inline void lslua_push(lua_State* L, const std::string& v) { lua_pushlstring(L, v.c_str(), v.size()); }

#if defined(__arm__) || defined(_WIN32)
inline void lslua_push(lua_State* L, long v) { lslua_push_int64(L, (int64_t)v); }
inline void lslua_push(lua_State* L, unsigned long v) { lslua_push_uint64(L, (uint64_t)v); }
#endif

#ifdef __APPLE__
inline void lslua_push(lua_State* L, size_t v) { lslua_push_uint64(L, (uint64_t)v); }
#endif

LS_API void lslua_push(lua_State* L, const LuaFunction& fun);


/** 安全的增加引用。避免因value被回收(弱表)，造成lua获取数组长度错误。 */
LS_API int lslua_safe_ref(lua_State* L, int table);
LS_API void lslua_safe_unref(lua_State* L, int table, int ref);

template <typename T>
inline void lslua_register_var(lua_State *L, const char *field, const T& value)
{
    lua_pushstring(L, field);
    lslua_push(L, value);
    lua_rawset(L, -3);
}

NS_LS_END
