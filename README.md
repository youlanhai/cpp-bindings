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
