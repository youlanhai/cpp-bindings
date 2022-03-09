//////////////////////////////////////////////////////////////////////
/// Desc  LuaState
/// Time  2022/03/09
/// Author youlanhai
/// Home   https://github.com/youlanhai/cpp-bindings
//////////////////////////////////////////////////////////////////////
#pragma once

#include "IScript.hpp"
#include <functional>

NS_LS_BEGIN


class LuaDestroyEventNode
{
public:
    LuaDestroyEventNode* prev;
    LuaDestroyEventNode* next;
    std::function<void()> callback;
};


class LuaState : public IScript
{
public:
    SCRIPT_CLASS(LuaState)
    
    typedef LuaDestroyEventNode Node;

    LuaState();
    ~LuaState();

    Node* addListener(std::function<void()> callback);
    void removeListener(Node *handle);

    void emitDestroyEvents();

    static void registerInstance(lua_State *L);
    static LuaState* getInstance(lua_State *L);

private:
    virtual void onScriptEngineClose() override;

    Node    header_;
};

NS_LS_END
