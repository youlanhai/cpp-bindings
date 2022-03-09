#set code = "(self->" + $type_info.name + ")"
static int ${signature_name}_getter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    $type_info.parent.fullname* self = nullptr;
    if (!lslua_to_object(L, 1, self, "$lua_class_name", &tolua_err) ||
        nullptr == self)
    {
        lslua_error(L, &tolua_err, "$lua_class_name", "$registration_name", argc, 0);
        return 0;
    }
    // $type_info.value_type.origin_name
    $get_ret_push($type_info.value_type, $code);
    return 1;
}

static int ${signature_name}_setter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    $type_info.parent.fullname* self = nullptr;
    #set arg_name = $get_arg_name($type_info, "value")
    #set arg_type = $get_arg_typename($type_info)
    $arg_type $arg_name;
    #set arg_check = $get_arg_check($type_info, 2, "value")
    if (!lslua_to_object(L, 1, self, "$lua_class_name", &tolua_err)  ||
        nullptr == self ||
        !$arg_check)
    {
        lslua_error(L, &tolua_err, "$lua_class_name", "$registration_name", argc, 1);
        return 0;
    }
    self->$type_info.name = std::move($arg_name);
    return 0;
}
