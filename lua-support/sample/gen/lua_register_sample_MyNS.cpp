#include "LuaWrap.hpp"
#include "../sample.hpp"
USING_NS_LS

static int lua_MyNS_counter_getter(lua_State* L)
{
    // int
    lua_pushinteger(L, (lua_Integer)MyNS::counter);
    return 1;
}

static int lua_MyNS_counter_setter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L);
    if (!lslua_to_number(L, 1, MyNS::counter, &tolua_err))
    {
        lslua_error(L, &tolua_err, "myns", "counter", argc, 1);
    }
    return 0;
}

static int lua_MyNS_createMyClass(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L);
    if (argc == 0)
    {
        if (true)
        {
            MyNS::MyClass * ret = MyNS::createMyClass();
            lslua_push_object(L, (MyNS::MyClass*)ret, "myns.MyClass");
            return 1;
        }
    }
    if (argc == 1)
    {
        char* name_;
        if (lslua_to_string(L, 1, name_, &tolua_err))
        {
            MyNS::MyClass * ret = MyNS::createMyClass(name_);
            lslua_push_object(L, (MyNS::MyClass*)ret, "myns.MyClass");
            return 1;
        }
    }
    if (argc == 2)
    {
        char* name_;
        int age_;
        if (lslua_to_string(L, 1, name_, &tolua_err) &&
            lslua_to_number(L, 2, age_, &tolua_err))
        {
            MyNS::MyClass * ret = MyNS::createMyClass(name_, age_);
            lslua_push_object(L, (MyNS::MyClass*)ret, "myns.MyClass");
            return 1;
        }
    }
    lslua_error(L, &tolua_err, "myns", "createMyClass", argc, 0);
    return 0;
}

static int lua_MyNS_deleteMyClass(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L);
    if (argc == 1)
    {
        MyNS::MyClass* p_;
        if (lslua_to_object(L, 1, p_, "myns.MyClass", &tolua_err))
        {
            MyNS::deleteMyClass(p_);
            return 0;
        }
    }
    lslua_error(L, &tolua_err, "myns", "deleteMyClass", argc, 1);
    return 0;
}

static int lua_MyNS_print(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L);
    if (argc == 1)
    {
        MyNS::MyClass* c_;
        if (lslua_to_object(L, 1, c_, "myns.MyClass", &tolua_err))
        {
            MyNS::print(c_);
            return 0;
        }
    }
    lslua_error(L, &tolua_err, "myns", "print", argc, 1);
    return 0;
}

static int lua_MyNS_print2(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L);
    if (argc == 1)
    {
        MyNS::MyClass* c_;
        if (lslua_to_object(L, 1, c_, "myns.MyClass", &tolua_err))
        {
            MyNS::print2(c_);
            return 0;
        }
    }
    lslua_error(L, &tolua_err, "myns", "print2", argc, 1);
    return 0;
}

static int lua_MyNS_print4(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L);
    if (argc == 1)
    {
        MyNS::MyClass *c_;
        if (lslua_to_value(L, 1, c_, "myns.MyClass", &tolua_err))
        {
            MyNS::print4(*c_);
            return 0;
        }
    }
    lslua_error(L, &tolua_err, "myns", "print4", argc, 1);
    return 0;
}

static int lua_MyNS_print5(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L);
    if (argc == 1)
    {
        MyNS::MyClass *c_;
        if (lslua_to_value(L, 1, c_, "myns.MyClass", &tolua_err))
        {
            MyNS::print5(*c_);
            return 0;
        }
    }
    lslua_error(L, &tolua_err, "myns", "print5", argc, 1);
    return 0;
}

static int lua_MyNS_print6(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L);
    if (argc == 1)
    {
        MyNS::MyClass *c_;
        if (lslua_to_value(L, 1, c_, "myns.MyClass", &tolua_err))
        {
            MyNS::print6(*c_);
            return 0;
        }
    }
    if (argc == 2)
    {
        MyNS::MyClass *c_;
        int a_;
        if (lslua_to_value(L, 1, c_, "myns.MyClass", &tolua_err) &&
            lslua_to_number(L, 2, a_, &tolua_err))
        {
            MyNS::print6(*c_, a_);
            return 0;
        }
    }
    lslua_error(L, &tolua_err, "myns", "print6", argc, 2);
    return 0;
}

int lua_register_sample_MyNS_BaseClassA(lua_State* L);
int lua_register_sample_MyNS_BaseClassB(lua_State* L);
int lua_register_sample_MyNS_MyClass(lua_State* L);
int lua_register_sample_MyNS_SafeClass(lua_State* L);

int lua_register_sample_MyNS(lua_State* L)
{
    if(lslua_beginmodule(L, "myns"))
    {
        lua_register_sample_MyNS_BaseClassA(L);
        lua_register_sample_MyNS_BaseClassB(L);
        lua_register_sample_MyNS_MyClass(L);
        lua_register_sample_MyNS_SafeClass(L);

        lslua_register_function(L, "createMyClass", lua_MyNS_createMyClass);
        lslua_register_function(L, "deleteMyClass", lua_MyNS_deleteMyClass);
        lslua_register_function(L, "print", lua_MyNS_print);
        lslua_register_function(L, "print2", lua_MyNS_print2);
        lslua_register_function(L, "print4", lua_MyNS_print4);
        lslua_register_function(L, "print5", lua_MyNS_print5);
        lslua_register_function(L, "print6", lua_MyNS_print6);

        lslua_register_getset(L, "counter", lua_MyNS_counter_getter, lua_MyNS_counter_setter);
        lua_pushstring(L, "version");
        lua_pushstring(L, MyNS::version);
        lua_rawset(L, -3);

        lslua_endmodule(L);
    }
    return 1;
}

static int lua_MyNS_BaseClassA_id_getter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::BaseClassA* self = nullptr;
    if (!lslua_to_object(L, 1, self, "myns.BaseClassA", &tolua_err) ||
        nullptr == self)
    {
        lslua_error(L, &tolua_err, "myns.BaseClassA", "id", argc, 0);
        return 0;
    }
    // int
    lua_pushinteger(L, (lua_Integer)(self->id));
    return 1;
}

static int lua_MyNS_BaseClassA_id_setter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::BaseClassA* self = nullptr;
    int value;
    if (!lslua_to_object(L, 1, self, "myns.BaseClassA", &tolua_err)  ||
        nullptr == self ||
        !lslua_to_number(L, 2, value, &tolua_err))
    {
        lslua_error(L, &tolua_err, "myns.BaseClassA", "id", argc, 1);
        return 0;
    }
    self->id = std::move(value);
    return 0;
}

static int lua_MyNS_BaseClassA_getAge(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::BaseClassA* self = nullptr;
    if(!lslua_to_object(L, 1, self, "myns.BaseClassA", &tolua_err) || nullptr == self)
    {
        goto ERROR_LABEL;
    }
    if (argc == 0)
    {
        if (true)
        {
            int ret = self->getAge();
            lua_pushinteger(L, (lua_Integer)ret);
            return 1;
        }
    }
ERROR_LABEL:
    lslua_error(L, &tolua_err, "myns.BaseClassA", "getAge", argc, 0);
    return 0;
}

static int lua_MyNS_BaseClassA_sayHello(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::BaseClassA* self = nullptr;
    if(!lslua_to_object(L, 1, self, "myns.BaseClassA", &tolua_err) || nullptr == self)
    {
        goto ERROR_LABEL;
    }
    if (argc == 0)
    {
        if (true)
        {
            self->sayHello();
            return 0;
        }
    }
ERROR_LABEL:
    lslua_error(L, &tolua_err, "myns.BaseClassA", "sayHello", argc, 0);
    return 0;
}


int lua_register_sample_MyNS_BaseClassA(lua_State* L)
{
    if(lslua_beginclass(L, "BaseClassA", "myns.BaseClassA", ""))
    {

        lslua_register_function(L, "getAge", lua_MyNS_BaseClassA_getAge);
        lslua_register_function(L, "sayHello", lua_MyNS_BaseClassA_sayHello);

        lslua_register_getset(L, "id", lua_MyNS_BaseClassA_id_getter, lua_MyNS_BaseClassA_id_setter);

        lslua_endclass(L);

        lslua_register_type(typeid(MyNS::BaseClassA).name(), "myns.BaseClassA");
    }
    return 1;
}

static int lua_MyNS_BaseClassB_id_getter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::BaseClassB* self = nullptr;
    if (!lslua_to_object(L, 1, self, "myns.BaseClassB", &tolua_err) ||
        nullptr == self)
    {
        lslua_error(L, &tolua_err, "myns.BaseClassB", "id", argc, 0);
        return 0;
    }
    // int
    lua_pushinteger(L, (lua_Integer)(self->id));
    return 1;
}

static int lua_MyNS_BaseClassB_id_setter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::BaseClassB* self = nullptr;
    int value;
    if (!lslua_to_object(L, 1, self, "myns.BaseClassB", &tolua_err)  ||
        nullptr == self ||
        !lslua_to_number(L, 2, value, &tolua_err))
    {
        lslua_error(L, &tolua_err, "myns.BaseClassB", "id", argc, 1);
        return 0;
    }
    self->id = std::move(value);
    return 0;
}

static int lua_MyNS_BaseClassB_sayHaha(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::BaseClassB* self = nullptr;
    if(!lslua_to_object(L, 1, self, "myns.BaseClassB", &tolua_err) || nullptr == self)
    {
        goto ERROR_LABEL;
    }
    if (argc == 0)
    {
        if (true)
        {
            self->sayHaha();
            return 0;
        }
    }
ERROR_LABEL:
    lslua_error(L, &tolua_err, "myns.BaseClassB", "sayHaha", argc, 0);
    return 0;
}

static int lua_MyNS_BaseClassB_setAge(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::BaseClassB* self = nullptr;
    if(!lslua_to_object(L, 1, self, "myns.BaseClassB", &tolua_err) || nullptr == self)
    {
        goto ERROR_LABEL;
    }
    if (argc == 1)
    {
        int age_;
        if (lslua_to_number(L, 2, age_, &tolua_err))
        {
            self->setAge(age_);
            return 0;
        }
    }
ERROR_LABEL:
    lslua_error(L, &tolua_err, "myns.BaseClassB", "setAge", argc, 1);
    return 0;
}


int lua_register_sample_MyNS_BaseClassB(lua_State* L)
{
    if(lslua_beginclass(L, "BaseClassB", "myns.BaseClassB", ""))
    {

        lslua_register_function(L, "sayHaha", lua_MyNS_BaseClassB_sayHaha);
        lslua_register_function(L, "setAge", lua_MyNS_BaseClassB_setAge);

        lslua_register_getset(L, "id", lua_MyNS_BaseClassB_id_getter, lua_MyNS_BaseClassB_id_setter);

        lslua_endclass(L);

        lslua_register_type(typeid(MyNS::BaseClassB).name(), "myns.BaseClassB");
    }
    return 1;
}

static int lua_MyNS_MyClass_id_getter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::MyClass* self = nullptr;
    if (!lslua_to_object(L, 1, self, "myns.MyClass", &tolua_err) ||
        nullptr == self)
    {
        lslua_error(L, &tolua_err, "myns.MyClass", "id", argc, 0);
        return 0;
    }
    // int
    lua_pushinteger(L, (lua_Integer)(self->id));
    return 1;
}

static int lua_MyNS_MyClass_id_setter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::MyClass* self = nullptr;
    int value;
    if (!lslua_to_object(L, 1, self, "myns.MyClass", &tolua_err)  ||
        nullptr == self ||
        !lslua_to_number(L, 2, value, &tolua_err))
    {
        lslua_error(L, &tolua_err, "myns.MyClass", "id", argc, 1);
        return 0;
    }
    self->id = std::move(value);
    return 0;
}

static int lua_MyNS_MyClass_MyClass(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    if(!lslua_is_class(L, 1, "myns.MyClass"))
    {
        goto ERROR_LABEL;
    }
    if (argc == 0)
    {
        if (true)
        {
            MyNS::MyClass *self = new MyNS::MyClass();
            LSUserData *p = lslua_push_object(L, self, 1);
            lslua_bind_free_method(p, self);
            return 1;
        }
    }
    if (argc == 1)
    {
        std::string name_;
        if (lslua_to_string(L, 2, name_, &tolua_err))
        {
            MyNS::MyClass *self = new MyNS::MyClass(name_);
            LSUserData *p = lslua_push_object(L, self, 1);
            lslua_bind_free_method(p, self);
            return 1;
        }
    }
    if (argc == 2)
    {
        std::string name_;
        int age_;
        if (lslua_to_string(L, 2, name_, &tolua_err) &&
            lslua_to_number(L, 3, age_, &tolua_err))
        {
            MyNS::MyClass *self = new MyNS::MyClass(name_, age_);
            LSUserData *p = lslua_push_object(L, self, 1);
            lslua_bind_free_method(p, self);
            return 1;
        }
    }
ERROR_LABEL:
    lslua_error(L, &tolua_err, "myns.MyClass", "MyClass", argc, 2);
    return 0;
}

static int lua_MyNS_MyClass_create(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L);
    if (argc == 0)
    {
        if (true)
        {
            MyNS::MyClass * ret = MyNS::MyClass::create();
            lslua_push_object(L, (MyNS::MyClass*)ret, "myns.MyClass");
            return 1;
        }
    }
    lslua_error(L, &tolua_err, "myns.MyClass", "create", argc, 0);
    return 0;
}

static int lua_MyNS_MyClass_getName(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::MyClass* self = nullptr;
    if(!lslua_to_object(L, 1, self, "myns.MyClass", &tolua_err) || nullptr == self)
    {
        goto ERROR_LABEL;
    }
    if (argc == 0)
    {
        if (true)
        {
            const std::string & ret = self->getName();
            lua_pushlstring(L, ret.c_str(), ret.size());
            return 1;
        }
    }
ERROR_LABEL:
    lslua_error(L, &tolua_err, "myns.MyClass", "get_name", argc, 0);
    return 0;
}

static int lua_MyNS_MyClass_setName(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::MyClass* self = nullptr;
    if(!lslua_to_object(L, 1, self, "myns.MyClass", &tolua_err) || nullptr == self)
    {
        goto ERROR_LABEL;
    }
    if (argc == 1)
    {
        std::string name_;
        if (lslua_to_string(L, 2, name_, &tolua_err))
        {
            self->setName(name_);
            return 0;
        }
    }
ERROR_LABEL:
    lslua_error(L, &tolua_err, "myns.MyClass", "set_name", argc, 1);
    return 0;
}


int lua_register_sample_MyNS_MyClass(lua_State* L)
{
    if(lslua_beginclass(L, "MyClass", "myns.MyClass", "myns.BaseClassA"))
    {

        lslua_register_new(L, lua_MyNS_MyClass_MyClass);
        lslua_register_function(L, "create", lua_MyNS_MyClass_create);
        lslua_register_function(L, "get_name", lua_MyNS_MyClass_getName);
        lslua_register_function(L, "set_name", lua_MyNS_MyClass_setName);

        lslua_register_getset(L, "id", lua_MyNS_MyClass_id_getter, lua_MyNS_MyClass_id_setter);

        lslua_endclass(L);

        lslua_register_type(typeid(MyNS::MyClass).name(), "myns.MyClass");
    }
    return 1;
}

static int lua_MyNS_SafeClass_id_getter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::SafeClass* self = nullptr;
    if (!lslua_to_object(L, 1, self, "myns.SafeClass", &tolua_err) ||
        nullptr == self)
    {
        lslua_error(L, &tolua_err, "myns.SafeClass", "id", argc, 0);
        return 0;
    }
    // int
    lua_pushinteger(L, (lua_Integer)(self->id));
    return 1;
}

static int lua_MyNS_SafeClass_id_setter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::SafeClass* self = nullptr;
    int value;
    if (!lslua_to_object(L, 1, self, "myns.SafeClass", &tolua_err)  ||
        nullptr == self ||
        !lslua_to_number(L, 2, value, &tolua_err))
    {
        lslua_error(L, &tolua_err, "myns.SafeClass", "id", argc, 1);
        return 0;
    }
    self->id = std::move(value);
    return 0;
}

static int lua_MyNS_SafeClass_name_getter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::SafeClass* self = nullptr;
    if (!lslua_to_object(L, 1, self, "myns.SafeClass", &tolua_err) ||
        nullptr == self)
    {
        lslua_error(L, &tolua_err, "myns.SafeClass", "name", argc, 0);
        return 0;
    }
    // std::string
    lua_pushlstring(L, (self->name).c_str(), (self->name).size());
    return 1;
}

static int lua_MyNS_SafeClass_name_setter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::SafeClass* self = nullptr;
    std::string value;
    if (!lslua_to_object(L, 1, self, "myns.SafeClass", &tolua_err)  ||
        nullptr == self ||
        !lslua_to_string(L, 2, value, &tolua_err))
    {
        lslua_error(L, &tolua_err, "myns.SafeClass", "name", argc, 1);
        return 0;
    }
    self->name = std::move(value);
    return 0;
}

static int lua_MyNS_SafeClass_onEnter_getter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::SafeClass* self = nullptr;
    if (!lslua_to_object(L, 1, self, "myns.SafeClass", &tolua_err) ||
        nullptr == self)
    {
        lslua_error(L, &tolua_err, "myns.SafeClass", "onEnter", argc, 0);
        return 0;
    }
    // lslua::LuaFunction
    lslua_push(L, (self->onEnter));
    return 1;
}

static int lua_MyNS_SafeClass_onEnter_setter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::SafeClass* self = nullptr;
    lslua::LuaFunction value;
    if (!lslua_to_object(L, 1, self, "myns.SafeClass", &tolua_err)  ||
        nullptr == self ||
        !lslua_to_function(L, 2, value, &tolua_err))
    {
        lslua_error(L, &tolua_err, "myns.SafeClass", "onEnter", argc, 1);
        return 0;
    }
    self->onEnter = std::move(value);
    return 0;
}

static int lua_MyNS_SafeClass_onExit_getter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::SafeClass* self = nullptr;
    if (!lslua_to_object(L, 1, self, "myns.SafeClass", &tolua_err) ||
        nullptr == self)
    {
        lslua_error(L, &tolua_err, "myns.SafeClass", "onExit", argc, 0);
        return 0;
    }
    // lslua::LuaFunction
    lslua_push(L, (self->onExit));
    return 1;
}

static int lua_MyNS_SafeClass_onExit_setter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::SafeClass* self = nullptr;
    lslua::LuaFunction value;
    if (!lslua_to_object(L, 1, self, "myns.SafeClass", &tolua_err)  ||
        nullptr == self ||
        !lslua_to_function(L, 2, value, &tolua_err))
    {
        lslua_error(L, &tolua_err, "myns.SafeClass", "onExit", argc, 1);
        return 0;
    }
    self->onExit = std::move(value);
    return 0;
}

static int lua_MyNS_SafeClass_SafeClass(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    if(!lslua_is_class(L, 1, "myns.SafeClass"))
    {
        goto ERROR_LABEL;
    }
    if (argc == 0)
    {
        if (true)
        {
            MyNS::SafeClass *self = new MyNS::SafeClass();
            LSUserData *p = lslua_push_object(L, self, 1);
            lslua_bind_free_method(p, self);
            return 1;
        }
    }
ERROR_LABEL:
    lslua_error(L, &tolua_err, "myns.SafeClass", "SafeClass", argc, 0);
    return 0;
}

static int lua_MyNS_SafeClass_ScriptClass(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L);
    if (argc == 0)
    {
        if (true)
        {
            const char * ret = MyNS::SafeClass::ScriptClass();
            lua_pushstring(L, ret);
            return 1;
        }
    }
    lslua_error(L, &tolua_err, "myns.SafeClass", "ScriptClass", argc, 0);
    return 0;
}

static int lua_MyNS_SafeClass_enter(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::SafeClass* self = nullptr;
    if(!lslua_to_object(L, 1, self, "myns.SafeClass", &tolua_err) || nullptr == self)
    {
        goto ERROR_LABEL;
    }
    if (argc == 0)
    {
        if (true)
        {
            self->enter();
            return 0;
        }
    }
ERROR_LABEL:
    lslua_error(L, &tolua_err, "myns.SafeClass", "enter", argc, 0);
    return 0;
}

static int lua_MyNS_SafeClass_exit(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::SafeClass* self = nullptr;
    if(!lslua_to_object(L, 1, self, "myns.SafeClass", &tolua_err) || nullptr == self)
    {
        goto ERROR_LABEL;
    }
    if (argc == 0)
    {
        if (true)
        {
            self->exit();
            return 0;
        }
    }
ERROR_LABEL:
    lslua_error(L, &tolua_err, "myns.SafeClass", "exit", argc, 0);
    return 0;
}

static int lua_MyNS_SafeClass_getScriptClass(lua_State* L)
{
    LSLuaError tolua_err;
    int argc = lua_gettop(L) - 1;
    MyNS::SafeClass* self = nullptr;
    if(!lslua_to_object(L, 1, self, "myns.SafeClass", &tolua_err) || nullptr == self)
    {
        goto ERROR_LABEL;
    }
    if (argc == 0)
    {
        if (true)
        {
            const char * ret = self->getScriptClass();
            lua_pushstring(L, ret);
            return 1;
        }
    }
ERROR_LABEL:
    lslua_error(L, &tolua_err, "myns.SafeClass", "getScriptClass", argc, 0);
    return 0;
}


int lua_register_sample_MyNS_SafeClass(lua_State* L)
{
    if(lslua_beginclass(L, "SafeClass", "myns.SafeClass", ""))
    {

        lslua_register_new(L, lua_MyNS_SafeClass_SafeClass);
        lslua_register_function(L, "ScriptClass", lua_MyNS_SafeClass_ScriptClass);
        lslua_register_function(L, "enter", lua_MyNS_SafeClass_enter);
        lslua_register_function(L, "exit", lua_MyNS_SafeClass_exit);
        lslua_register_function(L, "getScriptClass", lua_MyNS_SafeClass_getScriptClass);

        lslua_register_getset(L, "id", lua_MyNS_SafeClass_id_getter, lua_MyNS_SafeClass_id_setter);
        lslua_register_getset(L, "name", lua_MyNS_SafeClass_name_getter, lua_MyNS_SafeClass_name_setter);
        lslua_register_getset(L, "onEnter", lua_MyNS_SafeClass_onEnter_getter, lua_MyNS_SafeClass_onEnter_setter);
        lslua_register_getset(L, "onExit", lua_MyNS_SafeClass_onExit_getter, lua_MyNS_SafeClass_onExit_setter);

        lslua_endclass(L);

        lslua_register_type(typeid(MyNS::SafeClass).name(), "myns.SafeClass");
    }
    return 1;
}
