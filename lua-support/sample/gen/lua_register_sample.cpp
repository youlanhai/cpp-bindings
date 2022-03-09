#include "LuaWrap.hpp"
#include "../sample.hpp"
USING_NS_LS

static int lua_simpleFun(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L);
    if (argc == 1)
    {
        int value_;
        if (lslua_to_number(L, 1, value_, &tolua_err))
        {
            simpleFun(value_);
            return 0;
        }
    }
    lslua_error(L, &tolua_err, "", "simpleFun", argc, 1);
    return 0;
}

int lua_register_sample_EAges(lua_State* L);
int lua_register_sample_MyNS(lua_State* L);

int lua_register_sample(lua_State* L)
{
    if(lslua_beginmodule(L, ""))
    {
        lua_register_sample_EAges(L);
        lua_register_sample_MyNS(L);

        lslua_register_function(L, "simpleFun", lua_simpleFun);


        lslua_endmodule(L);
    }
    return 1;
}


int lua_register_sample_EAges(lua_State* L)
{
    if(lslua_beginmodule(L, "EAges"))
    {
        lslua_register_var(L, "EAges_1", (int)EAges::EAges_1);
        lslua_register_var(L, "EAges_20", (int)EAges::EAges_20);
        lslua_endmodule(L);
    }
    return 1;
}
