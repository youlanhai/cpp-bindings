c++接口解析工具
================

C++接口绑定到其他语言的工具。目前支持Lua

# 环境搭建
需要python3，并且安装以下python插件:
- libclang (注意不要安装成clang了！)
- Cheetah3

```
pip install libclang Cheetah3
```

# 如何运行
`python cparser/main.py -c lua-support/sample/sample_config.py -o lua-support/sample/gen`

## 如何运行测试工程
1. 安装CMake工具。
2. Windows系统使用Visual Studio，Mac系统使用XCode。
3. git更新lua子模块:
```
git submodule update --init
```
4. 在lua-support目录下新建一个build目录，进入build目录执行cmake命令，生成工程文件。
```
cd lua-support
mkdir build
cd build
cmake -G "Visual Studio 15 Win64" ..
```
5. 编译工程，生成sample.exe。执行测试lua文件:
```
sample.exe test.lua
```
