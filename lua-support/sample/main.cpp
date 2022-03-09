#include "sample.hpp"
#include "LuaWrap.hpp"
#include "LuaState.hpp"

int lua_register_sample(lua_State* L);

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("no input script!\n");
        return -1;
    }


    simpleFun(EAges_20);

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    LuaState::registerInstance(L);
    lua_register_sample(L);

    int ret = luaL_dofile(L, argv[1]);
    lua_close(L);

    if (ret != 0)
    {
        printf("run lua file failed!\n");
        return -1;
    }
    return 0;
}
