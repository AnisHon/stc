import argparse
import os
from collections import defaultdict
from datetime import datetime

from jinja2 import Template


class Property:
    def __init__(self, beg: int, end: int, tags: list[str]):
        self.beg = beg
        self.end = end
        self.tags = tags

    def translate_tags(self, str2num: dict[str, list[int]]):
        """
        tags转换成bit field数字
        """
        tag_nums = map(lambda x: str2num[x], self.tags)  # list[list[int]]
        tag_num: list[int] = [sum(nums) for nums in zip(*tag_nums)]
        bit_num = 0
        for i, item in enumerate(reversed(tag_num)):
            bit_num += item << i

        return bit_num

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        tag_str = "; ".join(self.tags)
        return f"{self.beg}..{self.end}; {tag_str}"


class ICURange:
    def __init__(self, beg: int, end: int, tag: int, tag_strs: list[str]):
        self.beg = hex(beg)
        self.end = hex(end)
        self.tag = hex(tag)
        self.tag_str = "; ".join(tag_strs)

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return f"{{beg: {self.beg}, end: {self.end}, tag: {self.tag}}}"


def build_prop(range_: str, tags: list[str]) -> Property:
    """
    自动转换成数字类型
    """
    assert len(tags) >= 1

    range_ = range_.split("..")  # a..b 切分数字

    assert 1 <= len(range_) <= 2

    # 转成数字, 16进制
    range_ = list(map(lambda x: int(x, 16), range_))

    if len(range_) == 1:
        beg, end = range_[0], range_[0]
    else:
        beg, end = range_[0], range_[1]

    # 转换成标准大驼峰
    tags = list(map(lambda x: x.replace("_", ""), tags))

    return Property(beg, end, tags)


def do_strip(arr: list[str]):
    return list(map(lambda x: x.strip(), arr))


def resolve_properties(path: str) -> list[Property]:
    """
    将文件解析成 property
    """
    lines = []

    # 读文件
    with open(file=path, mode="r", encoding="utf-8") as props:
        lines.extend(props.readlines())

    # 流水线处理文件，过滤空行 注释 -> 切分 -> 去空格 -> 创建类型
    lines = filter(lambda x: not x.startswith("#") and not x.isspace(), lines)  # [str]
    lines = map(lambda x: x.split(";"), lines)  # [[str]]
    lines = map(lambda x: do_strip(do_strip(x)), lines)  # [[str]]

    props = map(lambda x: build_prop(x[0], x[1:]), lines)  # [Property]
    return list(props)


def collect_tags(props: list[Property]) -> list[str]:
    """
    收集所有标签，保证顺序确定
    """
    tags = set()
    for prop in props:
        for tag in prop.tags:
            tags.add(tag)

    # 排序确定顺序
    return sorted(list(tags))


def assign_bit_field(tags: list[str]) -> dict[str, list[int]]:
    """
    每个tag分配一个32位数bit位
    """
    str2num = {}
    for i, item in enumerate(tags):
        # 效率什么的不重要
        bit_num = [0] * 32
        bit_num[i] = 1
        bit_num.reverse()
        str2num[item] = bit_num

    return str2num


def combine_interval(props: list[Property]) -> list[Property]:
    """
    线性扫描切分区间，得到统一不重叠区间
    """
    events_ = []  # (区间边界，标签，进入T\退出F)
    for prop in props:
        events_.append((prop.beg, prop.tags, True))
        events_.append((prop.end + 1, prop.tags, False))  # 左闭右开

    # 排序
    events_.sort(key=lambda x: x[0])  #

    active_tags = defaultdict(int)
    combined: list[Property] = []

    prev = events_[0][0]
    for (edge, tags, is_enter) in events_:
        if prev != edge:
            curr_tags = []
            # 收集tag
            for k, v in active_tags.items():
                if v > 0:
                    curr_tags.append(k)

            # 如果是有效区间，增加新的interval
            if len(curr_tags) != 0:
                combined.append(Property(prev, edge - 1, curr_tags))

            # 更新前一个
            prev = edge

        # 修改计数
        if is_enter:
            for t in tags:
                active_tags[t] += 1
        else:
            for t in tags:
                active_tags[t] -= 1

    return combined


def generate_codes(template_source_path: str, template_header_path: str, include_path: str, namespace: str,
                   ranges: list[ICURange]) -> tuple[str, str]:
    with open(template_source_path, "r", encoding="utf-8") as f:
        template_source = Template(f.read())

    with open(template_header_path, "r", encoding="utf-8") as f:
        template_header = Template(f.read())

    context_source = {
        'namespace_name': namespace,
        'generated_time': datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
        'ranges': ranges,
        'include_path': include_path,
    }

    context_header = {
        'namespace_name': namespace,
        'generated_time': datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
        'ranges_len': len(ranges)
    }

    source = template_source.render(**context_source)
    header = template_header.render(**context_header)

    return source, header


def write_codes(path: str, code: str):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w+", encoding="utf-8") as f:
        f.write(code)
        print(f"已经写入: {path}")


def main():
    parser = argparse.ArgumentParser(description='生成unicode header代码')
    parser.add_argument('input', help='源码路径')
    parser.add_argument('--output_source', type=str, required=True, help='输出源码')
    parser.add_argument('--output_header', type=str, required=True, help='输出头')
    parser.add_argument('--template_source', type=str, required=True, help='源码模板')
    parser.add_argument('--template_header', type=str, required=True, help='头模版')
    parser.add_argument('--include_path', type=str, required=True, help='include 路径')
    parser.add_argument('--namespace', type=str, required=True, help='名称空间')
    args = parser.parse_args()

    output_source: str = args.output_source
    file_path: str = args.input
    output_header: str = args.output_header

    # 流水线
    props: list[Property] = resolve_properties(file_path)  # 读 property
    props = combine_interval(props)  # 合并
    tags: list[str] = collect_tags(props)  # 收集标签
    str2num: dict[str, list[int]] = assign_bit_field(tags)  # 映射标签
    entries = map(lambda x: (x, x.translate_tags(str2num)), props)  # 准备entry
    ranges: list[ICURange] = list(map(lambda x: ICURange(x[0].beg, x[0].beg, x[1], x[0].tags), entries))  # 生成最终区间
    source, header = generate_codes(args.template_source, args.template_header, args.include_path, args.namespace,
                                    ranges)  # 生成代码

    write_codes(output_source, source)
    write_codes(output_header, header)


if __name__ == '__main__':
    main()
else:
    raise RuntimeError("it's a script, not a module")
