# 仓库约定

这是一个用 C++23 实现的教学型编译器工具链。根目录采用 LLVM 单仓库的组织思路，前端和后端是同级子项目。近期目标是先打通 C 子集。

- `stcc/` 是 C/C++ 编译器前端，负责从源码到经过语义检查的 AST，以及将其转换为中间表示（IR）的前端逻辑。
- 后端目前处于规划阶段。首次实现时在根目录下建立同级子项目，负责从 IR 到目标代码，包括指令选择、寄存器分配和目标机器描述。
- 前后端通过 IR 交接。后端接口不暴露给 Lexer、Parser 或 Sema；后端也不依赖前端的 token、AST 节点或源码缓冲区。出现实际交接需求时再确定 IR 的表示和归属。
- 子项目拥有自己的 CMake 目标、依赖和测试。根目录保留跨项目约定、开发文档和共享工具配置；单个子项目的辅助代码就近维护。

当前构建入口是 `stcc/`。在仓库根目录首次执行 `uv sync --locked`，然后运行：

```sh
cmake -S stcc -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
