//////////////////////////////////////////////////////////////////////
/// Desc  LuaObject
/// Time  2022/03/09
/// Author youlanhai
/// Home   https://github.com/youlanhai/cpp-bindings
//////////////////////////////////////////////////////////////////////

#pragma once
#include "IScript.hpp"

NS_LS_BEGIN

/// @brief 简单的lua函数包装器，保存lua函数的引用。
/// @notice 需要调用destroy显示的销毁引用。
class LS_API LuaObject
{
protected:
    IScript*        state_ = nullptr;
    int             ref_ = 0;
public:
    static const LuaObject null;

    /// 构造一个空对象
    LuaObject::LuaObject()
    {
    }

    /// 通过lua解释器和函数引用，构造包装器
    LuaObject::LuaObject(lua_State* L, int ref)
    {
        setRef(L, ref);
    }

    /// 移动构造
    LuaObject::LuaObject(LuaObject&& other) noexcept
    {
        steal(other);
    }

    LuaObject::~LuaObject()
    {
        destroy();
    }

    LuaObject(const LuaObject&) = delete;
    const LuaObject& operator=(const LuaObject&) = delete;

    LuaObject& LuaObject::operator=(LuaObject&& other) noexcept
    {
        steal(other);
        return *this;
    }

    void steal(LuaObject& other);

    /// 判断当前引用的函数是否有效
    operator bool() const { return state_ != nullptr && state_->getLuaState() != nullptr; }

    lua_State* getLuaState() { return state_ != nullptr ? state_->getLuaState() : nullptr; }
    const lua_State* LuaObject::getLuaState() const { return const_cast<LuaObject*>(this)->getLuaState(); }

    /// 获取函数的引用
    int getRef() const { return ref_; }

    /// 设置函数引用。会释放旧的引用
    void setRef(lua_State *L, int ref);

    /// 释放lua函数的引用。
    /// @notice c++层需要手动调用该函数来释放lua函数引用，否则会造成内存泄露。
    virtual void destroy();

    /// 仅重置变量，不释放函数引用
    virtual void clear();

    /// 将当前引用的函数放在lua栈顶。
    /// @notice 该函数会先将错误处理函数push在栈顶，然后push当前引用的函数
    virtual void push();
};

NS_LS_END
