### 生成 `unicode_props` 代码

```shell
uv run script/unicode_gen.py resources/DerivedCoreProperties.txt \
  --output_source src/utils/generated/unicode_props.cpp \
  --output_header include/stc/utils/generated/unicode_props.h \
  --template_source resources/templates/unicode_props.cpp.jinja \
  --template_header resources/templates/unicode_props.h.jinja \
  --include_path stc/utils/generated/unicode_props.h \
  --namespace stc::utils::gen
```

### 生成 token 相关定义

```shell
uv run script/token_gen.py resources/token_def.txt \
  --template resources/templates/token_kinds.def.jinja \
  -o include/stc/lexer/generated/token_kinds.def
```