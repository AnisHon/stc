# STCC 前端开发约定

本目录负责 C/C++ 前端，优先实现 C 子集。当前已有 `source`、`lexer`、`utils` 模块。文件加载和宏定义登记已有实现；`Lexer::next_token()` 尚未完成，`TokenStream` 仍是骨架。Parser、Sema 和 AST 属于后续阶段。

## 修改入口与接口约束

- 模块头文件放在 `include/stcc/<模块>/`，实现放在 `src/<模块>/`。依赖声明在实际使用它的 CMake 目标上。公共头文件暴露的依赖用 PUBLIC，仅实现需要的用 PRIVATE。
- 源码归 `SourceManager` 管理。文件内偏移按字节计，`SourceRange` 为左闭右开区间。零值是无效 ID 或无效位置；有效文件的字节偏移 0 表示首字节。借用的源码视图必须在缓冲区有效期内使用。
- Lexer 每次扫描一个缓冲区，产出 `PPToken`。保留 `range`、`leading_space` 和 `start_of_line`，后两者供预处理使用。此阶段把关键字拼写识别为标识符，留到预处理后再分类。
- 预处理层负责指令、文件包含和宏展开。实现宏时同时保留 token 的拼写位置和调用位置；嵌套展开须能逐层回溯。Parser 只消费预处理后的 token，名称查找与类型检查交给 Sema，AST 节点按翻译单元管理。
- 用户输入错误通过返回值或诊断报告，断言只检查内部不变量。Lexer 遇到非法输入时应报告错误并推进游标，或明确结束扫描。
- token 表与 Unicode 表由构建生成。改数据时编辑 `resources/`，改输出格式时编辑 `templates/`，改生成算法时编辑 `utils/` 中的 Python 脚本。

## 构建与测试

以下命令从 **stcc 子目录** 执行。使用支持 C++23 的编译器与标准库、Ninja、uv，建议 CMake 3.25 或更新版本。

```sh
uv sync --locked
cmake -S . -B ../build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build ../build --target stcc_exec stcc_test
ctest --test-dir ../build --output-on-failure
```

Catch2 测试放在 `unitest/<模块>/`，新增测试源须加入 `stcc_test` 的源文件列表。定位相关测试除 token 种类外，还应断言字节范围；宏展开测试分别断言拼写位置和调用位置。
