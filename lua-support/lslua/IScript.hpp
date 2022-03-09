//////////////////////////////////////////////////////////////////////
/// Desc  IScript
/// Time  2022/03/09
/// Author youlanhai
/// Home   https://github.com/youlanhai/cpp-bindings
//////////////////////////////////////////////////////////////////////
#pragma once

#include "ls-config.hpp"
#include "lua.hpp"

NS_LS_BEGIN

#define LS_MAKE_STR(A) #A

#define SCRIPT_CLASS(CLASS)\
    typedef this_class base_class; \
    typedef CLASS this_class; \
    static const char* ScriptClass(){ return "lslua." #CLASS; } \
    virtual const char* getScriptClass() override { return ScriptClass(); }

#define SCRIPT_CLASS2(CLASS, MODULE)\
    typedef this_class base_class; \
    typedef CLASS this_class; \
    static const char* ScriptClass(){ return MODULE "." #CLASS; } \
    virtual const char* getScriptClass() override { return ScriptClass(); }


/** @brief 脚本对象基类。
 *  内存管理使用 引用计数+强引用+弱引用组合，来管理内存:
 *  C++端的对象，默认只记录lua端对象的弱引用，用于和lua交互。当lua端对象被回收时，
 *  gc函数会销毁C++端对象。如果想要在C++中持有lua端对象的强引用，调用@ref retain 方法，
 *  并且在使用完毕后，调用@ref release 方法。
 *
 */
class LS_API IScript
{
public:
    typedef IScript this_class;
    
    IScript();
    virtual ~IScript();
    
    /** @brief 初始化脚本对象，并把对象放在lua栈顶上。如果初始化失败，栈顶上什么也不放
     *
     *  @param L        lua解释器指针
     *  @return 如果没有找到类表，会回false，其他情况都返回true。
     *  @private
     */
    virtual bool initScript(lua_State *L);

    /** @private */
    lua_State* getLuaState(){ return luaState_; }

    /** @brief 增加引用计数。
     *
     * 当引用计数>0的时候，C++端的对象会持有lua端对象的强引用，避免lua端对象gc回收并清理，
     * 从而引发C++端对象也被回收。
     * @notice 该函数只能在C++中调用
     *  @private
     */
    void retain();
    
    /** @brief 减少引用计数
     *
     *  当引用计数为0的时候，C++端对象会释放掉lua端对象的强引用。当lua端对象被gc回收后，
     *  C++端对象也会被销毁。
     *  @private
     */
    void release();

    int getRefCount() const { return refCount_; }
    int getWeakRefHandle() const { return weakRefHandle_; }
    int getStrongRefHandle() const { return strongRefHandle_; }
    
    /// 获取对脚本类名称
    virtual const char* getScriptClass() = 0;

    virtual size_t getMemorySize() { return sizeof(*this); }

    /** 获得当前内存中的IScript对象数量 */
    static size_t getObjectCount();

    /** 调试用。用来追踪所有创建出来的IScript对象，仅在开启了ENABLE_TEST模式下才生效 */
    static IScript* getChainRoot();
    static size_t getChainCount();
    static void printChain();
    IScript* getChainNext();
    IScript* getChainPrev();

public:

    static const char* ScriptClass(){ return "lslua.IScript"; }

    /** @private 将全局弱表放在栈顶。全局弱表用于存贮对象的弱引用 */
    static int push_weaktable(lua_State *L);
    
    /** @private 将IScript类型的对象，放在lua栈顶 */
    static int push_script(lua_State *L, IScript *p);

    /** @private */
    static void free_script(IScript *self);

protected:

    virtual void onScriptEngineClose() {}

    /// lua解释器对象
    lua_State*  luaState_;

private:
    /// C++端的引用计数
    int         refCount_;
    /// C++对象对应的lua端对象句柄
    int         weakRefHandle_;

    /** 记录lua端对象的强引用，避免被自动gc。
     *  弱引用有一个潜在的问题: 弱元素被设置成了nil，但是gc回调函数会被延迟触发，
     *  导致中间阶段weakRefHandle记录的值变成了nil，如果此时发生push_script，
     *  将无法得到正确的值。这种情况仅会出现在C++侧有强引用的情况，否则此对象不可能在被gc回收后，又发生了复用。
     */
    int         strongRefHandle_;

#ifdef ENABLE_TEST
    IScript*    prev_;
    IScript*    next_;
#endif

private:
    IScript(const IScript&) = delete;
    const IScript& operator = (const IScript&) = delete;
};

NS_LS_END
