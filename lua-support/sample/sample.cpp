#include "sample.hpp"
#include <cstdio>
#include <string>

namespace MyNS
{
    int counter = 0;
    const char* version = "1.0.1";
}

void simpleFun(int value)
{
    auto *c = MyNS::createMyClass();
    c->setAge(value);
    c->setName("haha");
    MyNS::print(c);
    delete c;
}
