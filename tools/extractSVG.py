#!/usr/bin/env python3
"""
从 index.tsx 中提取所有 SVG 图标，生成 C++ 头文件 icons.h
使用方法：python extract_icons.py
"""

import re
import os

INPUT_FILE = "index.tsx"
OUTPUT_FILE = "icons.h"

def extract_svgs(content):
    """提取所有 SVG 组件，返回 {name: svg_string}"""
    # 匹配 const 或 export const 后跟名称（以 SVG 结尾），然后箭头函数，括号，然后是 <svg ...> ... </svg>
    # 注意：JSX 可能跨多行，使用 [\s\S] 匹配包括换行
    pattern = r'(?:export\s+)?const\s+(\w+SVG)\s*=\s*\(\)\s*=>\s*\(\s*((?:<svg[\s\S]*?<\/svg>))\s*\)'
    matches = re.findall(pattern, content)
    svg_dict = {}
    for name, svg in matches:
        # 去掉名称后缀 "SVG"，保留原名如 DarkMode
        key = name[:-3]  # 去掉最后一个 "SVG"
        svg_dict[key] = svg.strip()
    return svg_dict

def generate_cpp_header(svg_dict):
    """生成 C++ 头文件内容"""
    lines = [
        "#pragma once",
        "#include <string>",
        "#include <unordered_map>",
        "",
        "namespace Icons {",
        ""
    ]

    # 为每个图标生成一个 const char* 原始字符串字面量
    for name, svg in svg_dict.items():
        # 使用原始字符串字面量，分隔符取 "ICON" 避免与内容冲突
        # 但注意如果 SVG 内包含 )ICON" 则需变更，一般不会
        lines.append(f'    inline const char* const {name} = R"ICON(')
        lines.append(svg)  # 直接原样插入
        lines.append('    )ICON";')
        lines.append('')

    # 可选：生成一个映射表，方便按名称访问
    lines.append('    inline const std::unordered_map<std::string, const char*> All = {')
    for name in svg_dict.keys():
        lines.append(f'        {{"{name}", {name}}},')
    lines.append('    };')
    lines.append('')
    lines.append('} // namespace Icons')

    return "\n".join(lines)

def main():
    if not os.path.exists(INPUT_FILE):
        print(f"错误：找不到 {INPUT_FILE}，请将本脚本放在 index.tsx 同目录下。")
        return

    with open(INPUT_FILE, 'r', encoding='utf-8') as f:
        content = f.read()

    svgs = extract_svgs(content)
    if not svgs:
        print("警告：未提取到任何 SVG 图标，请检查正则匹配。")
        return

    print(f"提取到 {len(svgs)} 个图标：{', '.join(svgs.keys())}")

    header = generate_cpp_header(svgs)

    with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
        f.write(header)

    print(f"已生成 {OUTPUT_FILE}")

if __name__ == "__main__":
    main()