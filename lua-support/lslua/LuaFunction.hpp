//////////////////////////////////////////////////////////////////////
/// Desc  LuaFunction
/// Time  2022/03/09
/// Author youlanhai
/// Home   https://github.com/youlanhai/cpp-bindings
//////////////////////////////////////////////////////////////////////
#pragma once
#include "LuaObject.hpp"

NS_LS_BEGIN

/** Lua函数wrap。不同于LuaSampleFunction，在析构的时候会释放函数引用
 */
class LS_API LuaFunction : public LuaObject
{
    int         errorHandle_ = 0;
public:
    LuaFunction::LuaFunction()
    {
    }

    LuaFunction::LuaFunction(lua_State *L, int ref)
        : LuaObject(L, ref)
    {
    }

    LuaFunction(LuaFunction&& other) noexcept
    {
        steal(other);
    }

    LuaFunction& operator = (LuaFunction&& other) noexcept
    {
        steal(other);
        return *this;
    }

    LuaFunction(const LuaFunction&) = delete;
    const LuaFunction& operator=(const LuaFunction&) = delete;

    void push();

    /// 保护模式下，调用函数。
    bool pcall(int nargs, int nresults);

    void call()
    {
        if (0 == ref_)
        {
            return;
        }
        push();
        pcall(0, 0);
    }

    template<typename T0>
    void call(const T0& v0)
    {
        if (0 == ref_)
        {
            return;
        }
        push();
        lslua_push(getLuaState(), v0);
        pcall(1, 0);
    }

    template<typename T0, typename T1>
    void call(const T0& v0, const T1& v1)
    {
        if (0 == ref_)
        {
            return;
        }
        push();
        lua_State* L = getLuaState();
        lslua_push(L, v0);
        lslua_push(L, v1);
        pcall(2, 0);
    }

    template<typename T0, typename T1, typename T2>
    void call(const T0& v0, const T1& v1, const T2& v2)
    {
        if (0 == ref_)
        {
            return;
        }
        push();
        lua_State* L = getLuaState();
        lslua_push(L, v0);
        lslua_push(L, v1);
        lslua_push(L, v2);
        pcall(3, 0);
    }

    template<typename T0, typename T1, typename T2, typename T3>
    void call(const T0& v0, const T1& v1, const T2& v2, const T3& v3)
    {
        if (0 == ref_)
        {
            return;
        }
        push();
        lua_State* L = getLuaState();
        lslua_push(L, v0);
        lslua_push(L, v1);
        lslua_push(L, v2);
        lslua_push(L, v3);
        pcall(4, 0);
    }
};

NS_LS_END
