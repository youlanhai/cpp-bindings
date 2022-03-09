//////////////////////////////////////////////////////////////////////
/// Desc  LuaState
/// Time  2022/03/09
/// Author youlanhai
/// Home   https://github.com/youlanhai/cpp-bindings
//////////////////////////////////////////////////////////////////////
#include "LuaState.hpp"
#include "LuaWrap.hpp"
#include "LogTool.hpp"

#include <cassert>

NS_LS_BEGIN

static int LuaDestroyEventTag;

LuaState::LuaState()
{
    header_.prev = &header_;
    header_.next = &header_;
}

LuaState::~LuaState()
{
    LOG_INFO("LuaState destruct");
    emitDestroyEvents();
}

LuaDestroyEventNode* LuaState::addListener(std::function<void()> callback)
{
    Node *p = new Node();
    p->callback = callback;
    p->prev = &header_;
    p->next = header_.next;
    header_.next->prev = p;
    header_.next = p;
    return p;
}

void LuaState::removeListener(LuaDestroyEventNode *p)
{
    if(p == nullptr || p->prev == nullptr || p->next == nullptr)
    {
        return;
    }

    p->prev->next = p->next;
    p->next->prev = p->prev;
    
    p->prev = nullptr;
    p->next = nullptr;
    delete p;
}

void LuaState::emitDestroyEvents()
{
    Node *iter = header_.next;
    while(iter != &header_)
    {
        Node *p = iter;
        iter = iter->next;

        // 先移除结点。防止callback触发删除或添加逻辑
        p->prev->next = p->next;
        p->next->prev = p->prev;
        p->next = nullptr;
        p->prev = nullptr;

        p->callback();
        delete p;
    }
}

void LuaState::onScriptEngineClose()
{
    LOG_INFO("LuaState: script engine close");
    emitDestroyEvents();
}

void LuaState::registerInstance(lua_State *L)
{
    lua_pushlightuserdata(L, &LuaDestroyEventTag);
    LuaState* self = new LuaState();
    push_script(L, self);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

LuaState* LuaState::getInstance(lua_State *L)
{
    assert(L);
    lua_pushlightuserdata(L, &LuaDestroyEventTag);
    lua_rawget(L, LUA_REGISTRYINDEX);

    LuaState* self = nullptr;
    lslua_to_object(L, -1, self, ScriptClass(), nullptr);
    
    lua_pop(L, 1);
    return self;
}

NS_LS_END
