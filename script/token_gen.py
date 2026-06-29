import argparse
from datetime import datetime
from dataclasses import dataclass
from jinja2 import Template

from utils import write_codes

PREFIX = "STC_TOKEN_KIND_"
sinces = ['c89', 'c99', 'c11', 'extension']

name_mapping = {
    'pp_number': 'PPNumber'
}

container = {
    'special': [],
    'identifier': [],
    'literal': [],
    'pp': [],
    'keyword': [],
    'delimiter': [],
    'operator': [],
    'preprocessor': [],
    'digraph': []
}


@dataclass(frozen=True)
class TokenKindEntry:
    kind: str
    symbol: str  # 原始符号
    name: str  # 别名
    since: str  # 来源，版本或拓展


def snake_to_pascal(s: str) -> str:
    return ''.join(word.title() for word in s.split('_'))


def resolve_named(line: list[str]) -> TokenKindEntry:
    assert len(line) == 2
    kind = line[0]
    assert kind in ['special', 'identifier', 'literal', 'pp']
    symbol = line[1]
    name = symbol
    if name in name_mapping:
        name = name_mapping[name]
    else:
        name = snake_to_pascal(symbol)
    return TokenKindEntry(kind=kind, symbol=symbol, name=name, since="c89")


def resolve_keyword(line: list[str]) -> TokenKindEntry:
    assert 3 <= len(line) <= 4
    kind = line[0]
    assert kind == 'keyword'
    since = line[2]
    assert since in sinces
    symbol = line[1]
    name = line[3] if len(line) == 4 else f"kw_{symbol}"
    name = snake_to_pascal(name)
    return TokenKindEntry(kind=kind, symbol=symbol, name=name, since=since)


def resolve_symbol(line: list[str]) -> TokenKindEntry:
    assert len(line) == 3
    kind = line[0]
    assert kind in ['delimiter', 'operator', 'preprocessor', 'digraph']
    symbol = line[1][1:-1]
    name = snake_to_pascal(line[2])
    return TokenKindEntry(kind=kind, symbol=symbol, name=name, since="c89")


def resolve_property(path: str):
    """
    解析props，直接输出到全局变量
    """
    resolver = {
        'special': resolve_named,
        'identifier': resolve_named,
        'literal': resolve_named,
        'pp': resolve_named,
        'keyword': resolve_keyword,
        'delimiter': resolve_symbol,
        'operator': resolve_symbol,
        'preprocessor': resolve_symbol,
        'digraph': resolve_symbol
    }

    with (open(path, "r", encoding="utf-8") as f):
        lines = f.readlines()

    lines = filter(lambda x: not x.startswith("#") and not x.isspace(), lines)  # [str]
    lines = map(lambda x: x.split(), lines)
    for line in lines:
        assert len(line) >= 1
        prop = resolver[line[0]](line)
        container[line[0]].append(prop)


def generate_def_code(def_template_path: str) -> str:
    with open(def_template_path, "r", encoding="utf-8") as f:
        def_template = Template(f.read())

    # 宏的名称 <-> 对应的列表
    macro_lists = {f"{k.upper()}_LIST": v for k, v in container.items()}

    template_ctx = {
        'generated_time': datetime.now().strftime("%Y-%m-%d"),
        "prefix": PREFIX,
        "macro_lists": macro_lists,
    }

    return def_template.render(template_ctx)


def main():
    parser = argparse.ArgumentParser(description='生成unicode header代码')
    parser.add_argument('input', help='定义路径')
    parser.add_argument('--template', help='template')
    parser.add_argument('-o', type=str, required=True, help='输出定义')

    args = parser.parse_args()
    resolve_property(args.input)

    code = generate_def_code(args.template)

    write_codes(args.o, code)

    return 0


if __name__ == '__main__':
    SystemExit(main())
else:
    raise RuntimeError("it's a script, not a module")
