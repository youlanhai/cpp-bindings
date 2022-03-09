#include <cstdio>
#include <string>
#include "LuaWrap.hpp"
#include "LuaFunction.hpp"

USING_NS_LS

enum
{
    TEST_EMPTY_ENUM_0,
};

void testMethod();

namespace MyNS
{
    class BaseClassA
    {
    public:
        int id;

        BaseClassA()
        {}

        virtual ~BaseClassA()
        {}

        virtual int getAge() const
        {
            return 0;
        }

        void sayHello() const
        {}
    };

    class BaseClassB
    {
    public:
        int id;

        BaseClassB()
        {}

        virtual ~BaseClassB()
        {}

        virtual void setAge(int age)
        {}

        void sayHaha() const
        {}
    };

    /** test my class */
    class MyClass : public BaseClassA, public BaseClassB
    {
    private:
        // test name
        std::string name;

    protected:
        // test age
        int age = 0;

    public:
        int id = 0;

        MyClass()
        {
            printf("MyClass construct: %p\n", this);
        }

        MyClass(const std::string &name)
        {
            this->name = name;
        }

        MyClass(const std::string &name, int age)
        {
            this->name = name;
            this->age = age;
        }

        ~MyClass()
        {
            printf("MyClass destruct: %p\n", this);
        }

        // get user name
        const std::string& getName() const
        {
            return name;
        }

        /** set user name
         *  @param name  user name
         */
        void setName(const std::string &name)
        {
            this->name = name;
        }

        int getAge() const override
        {
            return age;
        }

        void setAge(int age) override
        {
            this->age = age;
        }

        static MyClass* create()
        {
            return new MyClass();
        }
    };

    extern int counter;
    extern const char* version;

    inline void print(MyClass *c)
    {
    }

    inline void print2(const MyClass *c)
    {
    }

    inline void print3(const MyClass *&c)
    {
    }

    inline void print4(MyClass c)
    {
    }

    inline void print5(MyClass &c)
    {
    }

    inline void print6(const MyClass &c)
    {
    }

    inline void print6(const MyClass &c, int a)
    {
    }

    inline MyClass* createMyClass(const char *name = "", int age = 0)
    {
        return new MyClass();
    }

    inline void deleteMyClass(MyClass* p)
    {
        delete p;
    }

    class SafeClass : public IScript
    {
    public:
        SCRIPT_CLASS2(SafeClass, "myns")

    public:
        SafeClass()
        {
            printf("SafeClass construct: %p\n", this);
        }

        ~SafeClass()
        {
            printf("SafeClass destruct: %p\n", this);
        }

        void enter()
        {
            onEnter.call(1, "2", 3.0f, true);
        }

        void exit()
        {
            onExit.call(this);
        }

        std::string name;
        int id = 0;

        LuaFunction onEnter;
        LuaFunction onExit;
    };
}

enum EAges
{
    EAges_1 = 1,
    EAges_20 = 20,
    EAges_100 = 100,
};

void simpleFun(int value);
