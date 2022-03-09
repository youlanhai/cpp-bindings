
int ${signature_name}(lua_State* L)
{
    if(lslua_beginmodule(L, "${registration_name}"))
    {
#for t in $inner_variables
        lslua_register_var(L, "$get_script_short_name($t)", (int)$t.fullname);
#end for
        lslua_endmodule(L);
    }
    return 1;
}
