//////////////////////////////////////////////////////////////////////
/// Desc  LuaFunction
/// Time  2022/03/09
/// Author youlanhai
/// Home   https://github.com/youlanhai/cpp-bindings
//////////////////////////////////////////////////////////////////////
#include "LuaFunction.hpp"
#include "LuaWrap.hpp"
#include "LogTool.hpp"

#include <cassert>

NS_LS_BEGIN

void LuaFunction::push()
{
    lua_State *L = getLuaState();
    
    errorHandle_ = lslua_push_error_fun(L);
    lua_getref(L, ref_);
}

bool LuaFunction::pcall(int nargs, int nresults)
{
    lua_State* L = getLuaState();

    if(!lua_isfunction(L, -nargs - 1))
    {
        LOG_ERROR("function must be pushed.");
        lua_pop(L, nargs);
        return false;
    }
    
    int ret = lua_pcall(L, nargs, nresults, errorHandle_);
    if(errorHandle_ != 0)
    {
        lua_remove(L, errorHandle_);
        errorHandle_ = 0;
    }
    
    if(ret != 0)
    {
        LOG_EXCEPTION("Failed execute: error %d, %s", ret, lua_tostring(L, -1));
        lua_pop(L, 1);
        return false;
    }
    return true;
}


NS_LS_END
