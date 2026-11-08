# stcc

STCC（simple toy chain）是一个学习 LLVM 模仿 clang 的练手项目

### 生成 `unicode_props` 代码

```shell
uv run utils/unicode_gen.py resources/DerivedCoreProperties.txt \
  --output_source ${GEN_SOURCE_DIR}/unicode_props.cpp \
  --output_header ${GEN_HEADER_DIR}/unicode_props.h \
  --template_source ${TEMPLATE_DIR}/unicode_props.cpp.jinja \
  --template_header ${TEMPLATE_DIR}/unicode_props.h.jinja \
  --include_path ${PATH}/unicode_props.h \
  --namespace stcc::utils::gen
```

### 生成 token 相关定义

```shell
uv run utils/token_gen.py resources/token_def.txt \
  --template ${TEMPLATE_DIR}/token_kinds.inc.jinja \
  -o ${GEN_HEADER_DIR}/token_kinds.inc
```
