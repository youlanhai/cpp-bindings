//////////////////////////////////////////////////////////////////////
/// Desc  IScript
/// Time  2022/03/09
/// Author youlanhai
/// Home   https://github.com/youlanhai/cpp-bindings
//////////////////////////////////////////////////////////////////////

#include "IScript.hpp"
#include "LuaWrap.hpp"
#include "LogTool.hpp"

#include <typeinfo>
#include <cassert>

NS_LS_BEGIN

static IScript *g_root = nullptr;
static size_t g_count = 0;

IScript::IScript()
: luaState_(nullptr)
, refCount_(0)
, weakRefHandle_(0)
, strongRefHandle_(0)
{
    ++g_count;
#ifdef ENABLE_TEST
    prev_ = nullptr;
    next_ = g_root;
    if (g_root)
    {
        g_root->prev_ = this;
    }
    g_root = this;
#endif
}

IScript::~IScript()
{
    // 如果发生断言，说明外面用delete强制删除了对象。
    assert(strongRefHandle_ == 0);
    
    if(weakRefHandle_ != 0)
    {
        push_weaktable(luaState_);
        lslua_safe_unref(luaState_, -1, weakRefHandle_);
        lua_pop(luaState_, 1);
    }

    --g_count;
#ifdef ENABLE_TEST
    if (prev_)
    {
        prev_->next_ = next_;
    }
    if (next_)
    {
        next_->prev_ = prev_;
    }
    if (this == g_root)
    {
        assert(prev_ == nullptr && "root node should't has prev node");
        g_root = next_;
    }
#endif
}

bool IScript::initScript(lua_State *L)
{
    if(luaState_ != nullptr)
    {
        return false;
    }
    
    luaState_ = L;
    LSUserData *p = lslua_new_userdata(L, getScriptClass(), 0);
    p->addr = this;
    p->freeMethod = (LSUserData::FreeMethod)free_script;
    
    // 将当前对象记录在弱表中
    push_weaktable(L);
    lua_pushvalue(L, -2);
    weakRefHandle_ = lslua_safe_ref(L, -2);
    lua_pop(L, 1); // pop weak table
    
    // 当引用计数大于0的时候，记录一个强引用
    if(refCount_ > 0)
    {
        lua_pushvalue(L, -1);
        strongRefHandle_ = lua_ref(luaState_, 1);
    }
    return true;
}

void IScript::retain()
{
    ++refCount_;
    // 如果lua端对象存在，则记录一个强引用
    if(strongRefHandle_ == 0 && weakRefHandle_ != 0)
    {
        push_weaktable(luaState_);
        lua_rawgeti(luaState_, -1, weakRefHandle_);
        strongRefHandle_ = lua_ref(luaState_, 1);
        lua_pop(luaState_, 1); // pop weaktable
    }
}

void IScript::release()
{
    --refCount_;
    assert(refCount_ >= 0);
    
    // 当引用计数为0的时候，移除强引用
    if(refCount_ == 0)
    {
        if(strongRefHandle_ != 0)
        {
            lua_unref(luaState_, strongRefHandle_);
            strongRefHandle_ = 0;
        }
        else
        {
            // 如果强引用不存在，说明lua端对象已经销毁了
            delete this;
        }
    }
}

/*static*/ void IScript::free_script(IScript *self)
{
    if(self->strongRefHandle_ != 0)
    {
        // 被动destroy。这种情况下，只有lua解释器销毁的时候，才会执行到这里。
        // 因为，C++对象一直持有lua对象的强引用，不会自动销毁。只能通过调用release来手动销毁。
        // 如果有内存泄露，检查这里。
        LOG_WARN("Maybe memory leak: %s", self->getScriptClass());
        self->strongRefHandle_ = 0;
        self->weakRefHandle_ = 0;
        self->luaState_ = nullptr;
        self->onScriptEngineClose();

        // 这里不能直接delete，因为C++端持有该对象的引用计数，需要代码手动调用release释放
    }
    else
    {
        //weakRefHandle_ 会在析构函数中释放
        delete self;
    }
}

static int weakTableMagicValue;


/*static*/ int IScript::push_weaktable(lua_State *L)
{
    lua_pushlightuserdata(L, &weakTableMagicValue);
    lua_rawget(L, LUA_REGISTRYINDEX);
    if(!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        
        // new weakTable
        lua_newtable(L);
     
        // reg[key] = weakTable
        lua_pushlightuserdata(L, &weakTableMagicValue);
        lua_pushvalue(L, -2);
        lua_rawset(L, LUA_REGISTRYINDEX);
        
        // metatable = {__mode = "kv"}
        lua_newtable(L);
        lua_pushstring(L, "v");
        lua_setfield(L, -2, "__mode");
        
        // setmetatable(weakTable, metatable)
        lua_setmetatable(L, -2);
    }
    return 1;
}

int IScript::push_script(lua_State *L, IScript *p)
{
    if(p == nullptr)
    {
        lua_pushnil(L);
        return 1;
    }
    
    if(p->weakRefHandle_ != 0)
    {
        push_weaktable(L);
        lua_rawgeti(L, -1, p->weakRefHandle_);
        lua_remove(L, -2);
        return 1;
    }
    
    if(p->initScript(L))
    {
        return 1;
    }
    
    lua_pushnil(L);
    return 1;
}

size_t IScript::getObjectCount()
{
    return g_count;
}

IScript* IScript::getChainRoot()
{
    return g_root;
}

size_t IScript::getChainCount()
{
#ifdef ENABLE_TEST
    size_t n = 0;
    IScript *p = g_root;
    while (p)
    {
        ++n;
        p = p->next_;
    }
    return n;
#else
    return 0;
#endif
}

IScript* IScript::getChainNext()
{
#ifdef ENABLE_TEST
    return next_;
#else
    return nullptr;
#endif
}

IScript* IScript::getChainPrev()
{
#ifdef ENABLE_TEST
    return prev_;
#else
    return nullptr;
#endif
}

void IScript::printChain()
{
#ifdef ENABLE_TEST
    IScript *p = g_root;
    while (p)
    {
        LOG_DEBUG("%p, %s", p, typeid(*p).name());
        p = p->next_;
    }
#endif
}
NS_LS_END
