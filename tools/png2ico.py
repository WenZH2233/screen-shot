#!/usr/bin/env python3
"""
PNG 转 ICO 转换工具
用法: python png2ico.py <PNG文件名>
输出: 在当前工作目录生成同名的 .ico 文件
"""

import sys
import os

try:
    from PIL import Image
except ImportError:
    print("错误: 未安装 Pillow 库，请运行: pip install Pillow")
    sys.exit(1)


def convert_png_to_ico(input_path):
    """将 PNG 转换为 ICO，输出到当前工作目录"""
    # 检查输入文件是否存在
    if not os.path.isfile(input_path):
        print(f"错误: 文件 '{input_path}' 不存在")
        sys.exit(1)

    # 打开图片
    try:
        img = Image.open(input_path)
    except Exception as e:
        print(f"无法打开图片: {e}")
        sys.exit(1)

    # 转为 RGBA 模式（支持透明通道）
    if img.mode != 'RGBA':
        img = img.convert('RGBA')

    # 生成输出路径（当前工作目录 + 同名 .ico）
    base_name = os.path.splitext(os.path.basename(input_path))[0]
    output_path = os.path.join(os.getcwd(), base_name + ".ico")

    # 常用 ICO 尺寸（Windows 图标通常包含多个尺寸）
    sizes = [
        (16, 16), (24, 24), (32, 32),
        (48, 48), (64, 64), (128, 128),
        (256, 256)
    ]

    # 保存为 ICO
    try:
        img.save(output_path, format='ICO', sizes=sizes)
        print(f"✅ 转换成功: {output_path}")
    except Exception as e:
        print(f"保存 ICO 失败: {e}")
        sys.exit(1)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("用法: python png2ico.py <PNG文件名>")
        sys.exit(1)

    convert_png_to_ico(sys.argv[1])