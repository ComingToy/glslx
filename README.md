# glslx: GLSL Language Server ✨  

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.10+-green.svg)](https://cmake.org/)

glslx is a GLSL language server based on the official [Khronos Group glslang](https://github.com/KhronosGroup/glslang) compiler library, providing comprehensive and accurate language support for GLSL shader development. 🚀  

## ✨ Features  

### ✅ Implemented Features  
- **Smart Code Completion**  
  - User-defined variables, structs, and functions  
  - Built-in variables, functions, and data types  
  - Language keywords and extension directives  
  - Struct member  
- **Precise Code Navigation**  
  - Go to Definition  
  - Document Outline View  
- **Real-time Error Diagnostics**  
  - Syntax and semantic checking via glslang  
- **Header File Support**  
  - Full handling of `#include` directives  

### 🚧 Planned Features  
- Semantic Tokens  
- Hover Documentation  
- Find References  

## 🔧 Build Guide  

### System Requirements  
- CMake 3.10+  
- C++17-compatible compiler  

### Build Steps  
1. Clone the repository (including submodules):  
   ```bash  
   git clone --recursive https://github.com/ComingToy/glslx.git  
   cd glslx  
   ```  

2. Configure and build the project:  
   ```bash  
   cmake -DENABLE_OPT=OFF -B build -S .  
   cmake --build build --parallel  
   ```  

3. Install (optional):  
   ```bash  
   sudo cp build/glslx /usr/local/bin/  
   ```  

## 📖 Usage  

### Neovim (coc.nvim) Configuration Example  

1. Edit the coc.nvim configuration file `~/.config/nvim/coc-settings.json`:  
   ```json  
   {  
     "languageserver": {  
       "glslang": {  
         "command": "/usr/local/bin/glslx",  
         "filetypes": ["glslx"]  
       }  
     }  
   }  
   ```  

2. Compilation Command Configuration:  
   Create a `compile_commands_glslx.json` file in the project root, following the [Compilation Database Specification](https://clang.llvm.org/docs/JSONCompilationDatabase.html). Example:  
   ```json  
   [  
     {  
       "directory": "/path/to/project",  
       "command": "glslc --target-env=vulkan1.3 -DInputType=float16_t -o output.spv -I /path/to/includes source.comp",  
       "file": "/path/to/project/source.comp",  
       "output": "output.spv"  
     }  
   ]  
   ```  

## 🎥 Feature Demos  

| Feature | Demo |  
|---------|------|  
| Code Diagnostics | ![Code Diagnostics](doc/diagnostic.gif) |  
| Extension Completion | ![Extension Completion](doc/completion_extension.gif) |  
| Function Completion | ![Function Completion](doc/completion_func.gif) |  
| Struct Member Completion | ![Struct Member Completion](doc/completion_field.gif) |  
| Goto Definition | ![Goto Definition](doc/definition.gif) |  

## 📜 License  

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for details.
