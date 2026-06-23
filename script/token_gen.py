import argparse
from datetime import datetime
from dataclasses import dataclass
from jinja2 import Template

from utils import write_codes

assign_op = ['=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '|=', '^=']
preprocessor = ['#', '##']

kinds = ['spacial', 'identifier', 'literal', 'keyword', 'delimiter', 'operator', 'preprocessor', 'digraph']
sinces = ['c89', 'c99', 'c11', 'extension']

specials = []
identifiers = []
literals = []
keywords = []
delimiters = []
operators = []
preprocessors = []
digraphs = []


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
    alias: str


@dataclass(frozen=True)
class SymbolProperty:
    """
    对应 delimiter operator preprocessor digraph
    """
    kind: str
    symbol: str
    name: str


def snake_to_pascal(s: str) -> str:
    return ''.join(word.title() for word in s.split('_'))


def resolve_named(line: list[str]) -> NamedProperty:
    assert len(line) == 2
    assert line[0] in ['special', 'identifier', 'literal']
    name = snake_to_pascal(line[1])
    return NamedProperty(line[0], name)


def resolve_keyword(line: list[str]) -> KeywordProperty:
    assert 3 <= len(line) <= 4
    assert line[0] == 'keyword'
    assert line[2] in sinces
    alias = line[3] if len(line) == 4 else f"kw_{line[1]}"
    alias = snake_to_pascal(alias)
    return KeywordProperty(line[1], line[2], alias)


def resolve_symbol(line: list[str]) -> SymbolProperty:
    assert len(line) == 3
    assert line[0] in ['delimiter', 'operator', 'preprocessor', 'digraph']
    symbol = line[1][1:-1]
    name = snake_to_pascal(line[2])
    return SymbolProperty(line[0], symbol, name)


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


def generator_header_code(namespace: str, token_kind_header: str) -> str:
    with open(token_kind_header, "r", encoding="utf-8") as f:
        header_template = Template(f.read())

    token_kind_ctx = {
        'generated_time': datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
        'namespace_name': namespace,
        'specials': specials,
        'identifiers': identifiers,
        'literals': literals,
        'keywords': keywords,
        'delimiters': delimiters,
        'operators': operators,
        'preprocessors': preprocessors,
        'digraphs': digraphs,
    }
    return header_template.render(**token_kind_ctx)


def generator_source_code(namespace: str, token_kind_source: str, include_path: str):
    with open(token_kind_source, "r", encoding="utf-8") as f:
        source_template = Template(f.read())
    assign_operators = filter(lambda x: x.symbol in assign_op, operators)
    keyword_sizes = list(set(map(lambda x: len(x.keyword), keywords)))
    keyword_sizes.sort()

    # 对所有符号的symbol首字母去重排序
    punctuators = delimiters + operators + digraphs
    letters = set(map(lambda x: x.symbol[0], punctuators))
    special_characters_start = sorted(list(letters))

    token_kind_ctx = {
        'generated_time': datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
        'include_path': include_path,
        'namespace_name': namespace,
        'specials': specials,
        'identifiers': identifiers,
        'literals': literals,
        'keywords': keywords,
        'delimiters': delimiters,
        'operators': operators,
        'preprocessors': preprocessors,
        'digraphs': digraphs,
        'assign_operators': assign_operators,
        'keyword_sizes': keyword_sizes,
        'special_characters_start': special_characters_start,
    }
    return source_template.render(**token_kind_ctx)


def main():
    parser = argparse.ArgumentParser(description='生成unicode header代码')
    parser.add_argument('input', help='定义路径')
    parser.add_argument('--namespace', type=str, required=True, help='输出源码')
    parser.add_argument('--token_kind_header_template', type=str, required=True, help='输出源码')
    parser.add_argument('--token_kind_source_template', type=str, required=True, help='输出源码')
    parser.add_argument('--token_kind_header', type=str, required=True, help='输出源码')
    parser.add_argument('--token_kind_source', type=str, required=True, help='输出源码')
    parser.add_argument('--include_path', type=str, required=True, help='导包路径')

    args = parser.parse_args()

    resolve_property(args.input)

    header_code = generator_header_code(args.namespace, args.token_kind_header_template)
    source_code = generator_source_code(args.namespace, args.token_kind_source_template, args.include_path)

    write_codes(args.token_kind_header, header_code)
    write_codes(args.token_kind_source, source_code)

    return 0


if __name__ == '__main__':
    SystemExit(main())
else:
    raise RuntimeError("it's a script, not a module")
