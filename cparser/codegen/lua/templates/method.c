static int ${signature_name}(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
#if $type_info.is_constructor
    if(!lslua_is_class(L, 1, "$lua_class_name"))
#else
    ${cpp_class_name}* self = nullptr;
    if(!lslua_to_object(L, 1, self, "$lua_class_name", &tolua_err) || nullptr == self)
#end if
    {
        goto ERROR_LABEL;
    }
#for func in $type_info.overloads
    #set arg_count = len($func.arguments)
    if (argc == ${arg_count})
    {
    #set arg_array = []
    #set check_list = []
    #for index in range($arg_count)
        #set $arg = $func.arguments[$index]
        ##
        #set arg_name = $get_arg_name($arg, $arg.name + "_")
        #silent $arg_array.append($arg_name)
        ##
        #set arg_type = $get_arg_typename($arg)
        $arg_type $arg_name;
        ##
        #set arg_check = $get_arg_check($arg, $index + 2, $arg.name + "_")
        #silent $check_list.append($arg_check)
    #end for
    #set arg_list = ", ".join($arg_array)
    #set check_codes = "true"
    #if len($check_list) > 0
        #set check_codes = " &&\n            ".join($check_list)
    #end if
        if ($check_codes)
        {
        #if $type_info.is_constructor
            ${cpp_class_name} *self = new ${cpp_class_name}($arg_list);
            LSUserData *p = lslua_push_object(L, self, 1);
            lslua_bind_free_method(p, self);
            return 1;
        #elif $func.return_type
            $func.return_type.origin_name ret = self->${func.name}($arg_list);
            $get_ret_push($func.return_type, "ret");
            return 1;
        #else
            self->${func.name}($arg_list);
            return 0;
        #end if
        }
    }
#end for
ERROR_LABEL:
    lslua_error(L, &tolua_err, "$lua_class_name", "$registration_name", argc, ${func.min_args});
    return 0;
}
