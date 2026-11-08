# ToyC 词法分析实验（完整可编译项目）

**依据：**上传的《ToyC 词法分析器实验要求》。**运行平台：**Linux / WSL / Ubuntu，CMake 3.12+，C++17 编译器。

## 一键编译及测试

```bash
cmake -S . -B build
cmake --build build
./build/compiler -token < tests/comprehensive.c > outputs/comprehensive.actual.token
./build/compiler -dump-dfa > outputs/dfa_transitions.tsv
python3 tests/run_tests.py
```

其中 `compiler` 生成在 `build/` 目录内。评测环境若要求 `./compiler -token`，可复制到项目根目录：

```bash
cp build/compiler ./compiler
./compiler -token < tests/comprehensive.c > test.token
```

程序从 **stdin** 读取，向 **stdout** 输出 Token，错误写入 **stderr**。有词法错误返回 1，无错误返回 0。

## 文件导览

| 路径 | 用途 |
|---|---|
| `CMakeLists.txt` | CMake 编译配置 |
| `src/main.cpp` | 完整 lexer：NFA 构建、子集构造、DFA 最小化、最长匹配、错误恢复 |
| `docs/01_regex_and_nfa.md` | 步骤1的理论、实现和结果解释 |
| `docs/02_scan_driver.md` | 步骤2扫描主循环 |
| `docs/03_longest_match.md` | 步骤3最长匹配 |
| `docs/04_comments_errors.md` | 步骤4注释和错误恢复 |
| `docs/dfa_illustration.png` | 词法自动机关键分支示意图 |
| `outputs/dfa_transitions.tsv` | 程序真实导出的最小化 DFA 状态转移 |
| `tests/*.c` | 综合输入和边界测试输入 |
| `outputs/*.token` | 真实运行产生的 Token 文件 |
| `outputs/*.stderr.txt` | 真实运行产生的错误诊断 |
| `outputs/test_summary.md` | 11 个测试的通过情况 |
| `docs/experiment_report.md` | 可用于提交报告的实验过程与结论 |

## 实现与指导书的对应关系

实现完整的 **NFA 合并 → DFA 子集构造 → 最小化 → 扫描** 流程。关键字、标识符、整数、运算符和界符共用同一个最小化 DFA。为准确处理注释未闭合和行列位置，注释及空白由扫描器单独跳过。DFA 最小化使用分区细化（保持接受标签和优先级），而非具体 Hopcroft 最小化过程。

## 实验指导材料两处笔误

- “源程序共 205 个 Token”与其提供的 `0..207` 输出不一致，实际为 **208**。
- “a+++b 切成 5 个 +”不成立；应为 **3 个 `+`**，合计 **5 个 Token**。

## 其他说明

代码生成的 Tokens 为纯 ASCII 编译器格式。源代码样例内可以有中文注释（以字节方式跳过）。非法 Unicode 标识符不在该 ToyC 字符集要求范围内。
