static int ${signature_name}_getter(lua_State* L)
{
    // $type_info.value_type.origin_name
    $get_ret_push($type_info.value_type, $type_info.fullname);
    return 1;
}

static int ${signature_name}_setter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L);
    #set arg_check = $get_arg_check($type_info, 1, $type_info.fullname)
    if (!$arg_check)
    {
        lslua_error(L, &tolua_err, "$registration_scope", "$registration_name", argc, 1);
    }
    return 0;
}
