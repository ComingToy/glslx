# glslx: Language Server For Glslang
> Powered by ❤️ Keep patching

glslx 是一个基于KhronosGroup官方[glslang](https://github.com/KhronosGroup/glslang)库开发的glslang语言服务器。

- [主要特性](#主要特性)
- [从源码编译](#从源码编译)
- [快速上手](#快速上手)

#### 主要特性：

- 跳转到定义
- 代码补全
    - 用户定义变量、结构体、函数
    - 内置变量、函数、类型 
    - 关键字
    - 拓展名
    - 结构体成员补全
- 基于glslang的代码诊断
- outline
- 支持`#include`包含头文件

规划中特性:
- semantic token
- hover
- reference

### 从源码编译

#### 步骤
先克隆仓库
```bash
git clone --recursive https://github.com/ComingToy/glslx.git
```
编译代码
```bash
cmake -DENABLE_OPT=OFF -B build -S .
cmake --build build --parallel
cp build/glslx /usr/local/bin/glslx # 可能需要root或者sudo权限
```

### 快速上手
这里以在neovim中使用coc.nvim配置为例。
先打开coc.nvim配置文件`~/.config/nvim/coc-settings.json`. 在`languageserver`中配置如下
```json
{
  ... // other config
  "languageserver": {
    "glslang":
    {
        "command": "/usr/local/bin/glslx",
        "filetypes": ["glslx"],
    },
  },
 ... // other config 
}
```
glslx从workspace根目录下读取`compile_commands_glslx.json`文件加载对应源码文件的编译指令，格式与[compile_commands.json](https://clang.llvm.org/docs/JSONCompilationDatabase.html)相同. 目前缺少针对glslc导出`compile_commands_glslx.json`的工具，只能依靠手动编写。一个例子如下:
```json
[
{
  "directory": "/home/ComingToy/github/glslx",
  "command": "glslc --target-env=vulkan1.3 -DInputType=float16_t -DOutputType=float16_t -o matmul_b0_tb_fp16a_v2.spv -I /home/ComingToy/github/glslx/examples/ /home/ComingToy/github/glslx/examples/matmul_broadcast1_fp16a_v2.comp",
  "file": "/home/ComingToy/github/glslx/examples/matmul_broadcast1_fp16a_v2.comp",
  "output": "matmul_b0_tb_fp16a_v2.spv"
}
]
```

### 演示
