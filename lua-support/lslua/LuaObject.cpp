//////////////////////////////////////////////////////////////////////
/// Desc  LuaObject
/// Time  2022/03/09
/// Author youlanhai
//////////////////////////////////////////////////////////////////////

#include "LuaObject.hpp"
#include "LuaState.hpp"

NS_LS_BEGIN

/*static*/ const LuaObject LuaObject::null;

void LuaObject::steal(LuaObject& other)
{
    if (this == &other)
    {
        return;
    }

    destroy();

    state_ = other.state_;
    ref_ = other.ref_;

    other.ref_ = 0;
    other.state_ = nullptr;
}

void LuaObject::destroy()
{
    if(ref_ != 0)
    {
        lua_State* L = getLuaState();
        if (L != nullptr)
        {
            lua_unref(L, ref_);
        }
        ref_ = 0;
    }

    if (state_ != nullptr)
    {
        state_->release();
        state_ = nullptr;
    }
}

void LuaObject::clear()
{
    ref_ = 0;
    destroy();
}

void LuaObject::setRef(lua_State *L, int ref)
{
    destroy();

    ref_ = ref;

    if (ref_ != 0 && L != nullptr)
    {
        state_ = LuaState::getInstance(L);
        state_->retain();
    }
}

void LuaObject::push()
{
    lua_State *L = getLuaState();
    if (L != nullptr)
    {
        lua_getref(L, ref_);
    }
}

NS_LS_END
