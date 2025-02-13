Import("env")

import os

# 定义要嵌入的文件
files_to_embed = [
    ("index_html", "include/web/src/index.html"),
]
# 将文件转换为 C/C++ 数组
for var_name, file_path in files_to_embed:
    with open(file_path, "r",encoding="utf-8") as f:
        content = f.read()

    # 生成头文件
    header_content="//该文件由embed_files.py自动生成,请勿手动修改\n"
    header_content = header_content+"#ifndef {var}_H\n".format(var=var_name)
    header_content = header_content+"#define {var}_H\n".format(var=var_name)

    header_content = header_content+f"const char {var_name}[] PROGMEM = R\"=====(\n{content}\n)=====\";\n"

    header_content = header_content+"#endif\n"
    file_name="include/web/c_header/{var}.h".format(var=var_name)
    # 写入头文件
    with open(file_name, "w", encoding="utf-8") as f:
        f.write(header_content)
    print(f"successfully generated {file_name}")