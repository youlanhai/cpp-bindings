#for t in $inner_types
int $get_register_name($t)(lua_State* L);
#end for

int ${signature_name}(lua_State* L)
{
    if(lslua_beginclass(L, "${registration_name}", "${registration_fullname}", "$base_class"))
    {
#for t in $inner_types
        $get_register_name($t)(L);
#end for

#for t in $inner_methods
    #if $t.is_constructor
        lslua_register_new(L, $get_register_fun_name($t));
    #else
        lslua_register_function(L, "$get_script_short_name($t)", $get_register_fun_name($t));
    #end if
#end for

#for t in $inner_variables
    #if $t.is_enum_item
        lslua_register_var(L, "$get_script_short_name($t)", (int)$t.fullname);
    #elif $t.is_const
        lua_pushstring(L, "$get_script_short_name($t)");
        $get_ret_push($t.value_type, $t.fullname);
        lua_rawset(L, -3);
    #else
        #set name_prefix = $get_register_var_name($t)
        lslua_register_getset(L, "$get_script_short_name($t)", ${name_prefix}_getter, ${name_prefix}_setter);
    #end if
#end for

        lslua_endclass(L);

        lslua_register_type(typeid($type_info.fullname).name(), "${registration_fullname}");
    }
    return 1;
}
