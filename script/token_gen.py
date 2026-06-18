import argparse
from dataclasses import dataclass
from typing import Optional

from jinja2 import Template

kinds = ['spacial', 'identifier', 'literal', 'keyword', 'delimiter', 'operator', 'preprocessor', 'digraph']
sinces = ['c89', 'c99', 'c11', 'extension']


@dataclass(frozen=True)
class NamedProperty:
    """
    对应 special identifier literal
    """
    kind: str
    name: str


@dataclass(frozen=True)
class KeywordProperty:
    """
    对应 keyword
    """
    keyword: str
    since: str
    alias: Optional[str]


@dataclass(frozen=True)
class SymbolProperty:
    """
    对应 delimiter operator preprocessor digraph
    """
    kind: str
    symbol: str
    name: str


specials = []
identifiers = []
literals = []
keywords = []
delimiters = []
operators = []
preprocessors = []
digraphs = []


def resolve_named(line: list[str]) -> NamedProperty:
    assert len(line) == 2
    assert line[0] in ['special', 'identifier', 'literal']
    return NamedProperty(line[0], line[1])


def resolve_keyword(line: list[str]) -> KeywordProperty:
    assert 3 <= len(line) <= 4
    assert line[0] == 'keyword'
    assert line[2] in sinces
    alias = line[3] if len(line) == 4 else None
    return KeywordProperty(line[1], line[2], alias)


def resolve_symbol(line: list[str]) -> SymbolProperty:
    assert len(line) == 3
    assert line[0] in ['delimiter', 'operator', 'preprocessor', 'digraph']
    return SymbolProperty(line[0], line[1], line[2])


def resolve_property(path: str):
    """
    解析props，直接输出到全局变量
    """
    resolver = {
        'special': resolve_named,
        'identifier': resolve_named,
        'literal': resolve_named,
        'keyword': resolve_keyword,
        'delimiter': resolve_symbol,
        'operator': resolve_symbol,
        'preprocessor': resolve_symbol,
        'digraph': resolve_symbol
    }

    container = {
        'special': specials,
        'identifier': identifiers,
        'literal': literals,
        'keyword': keywords,
        'delimiter': delimiters,
        'operator': operators,
        'preprocessor': preprocessors,
        'digraph': digraphs
    }

    with (open(path, "r", encoding="utf-8") as f):
        lines = f.readlines()

    lines = filter(lambda x: not x.startswith("#") and not x.isspace(), lines)  # [str]
    lines = map(lambda x: x.split(), lines)
    for line in lines:
        assert len(line) >= 1
        prop = resolver[line[0]](line)
        container[line[0]].append(prop)


def generator_toke_kind_code(namespace: str, token_kind: str):
    with open(token_kind, "r", encoding="utf-8") as f:
        token_kind = Template(f.read())
    token_kind_ctx = {
        'namespace_name': namespace,
        'specials': specials,
        'identifiers': identifiers,
        'literals': literals,
        'keywords': keywords,
        'delimiters': delimiters,
        'operators': operators,
        'preprocessors': preprocessors,
        'digraphs': digraphs
    }
    return token_kind.render(**token_kind_ctx)


def main():
    parser = argparse.ArgumentParser(description='生成unicode header代码')
    parser.add_argument('input', help='定义路径')
    parser.add_argument('--namespace', type=str, required=True, help='输出源码')
    parser.add_argument('--token_kind_template', type=str, required=True, help='输出源码')

    args = parser.parse_args()

    resolve_property(args.input)
    # print(specials, identifiers, literals, keywords, delimiters, operators, preprocessors, digraph)
    print(generator_toke_kind_code(args.namespace, args.token_kind_template))

    return 0


if __name__ == '__main__':
    SystemExit(main())
else:
    raise RuntimeError("it's a script, not a module")
