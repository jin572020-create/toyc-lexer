# ToyC 词法分析器实验报告

## 一、实验目的

实现 ToyC 词法分析器，将程序字符流转换为 Token 序列；掌握正则规则对应的有限自动机、Thompson 风格 NFA 构造、ε-closure 子集构造、DFA 最小化、最长匹配、注释过滤和错误定位。

## 二、实验环境

Linux/WSL/Ubuntu；C++17、CMake 3.12+、Python 3（仅用于自动化测试）。词法分析程序本身没有第三方依赖。

## 三、实验原理与数据结构

Token 内含 `kind`、`lexeme`、`line`、`column`。程序通过 NFA 规则合并构造统一 DFA，接受状态记录类别与规则优先级；运行时记录最后可接受位置以实现最长匹配；同长度优先级由规则顺序决定。

核心正则：`[_A-Za-z][_A-Za-z0-9]*`；`0|[1-9][0-9]*`。关键字、运算符、界符按实验要求设置。

## 四、实验步骤及实测结果

### 4.1 构造 DFA

`src/main.cpp` 构造各词法规则 NFA，以 ε 边连接合并起点，随后使用 ε-closure 与 move 进行子集构造，使用分区细化保持 Token 接受标签后最小化。运行 `./build/compiler -dump-dfa`，实际导出状态转移表 `outputs/dfa_transitions.tsv`。`docs/dfa_illustration.png` 为局部原理图，不替代完整表。

### 4.2 扫描主循环

调用 `./build/compiler -token < tests/comprehensive.c > outputs/comprehensive.actual.token`。实测生成 **208** 条 Token（编号 `0..207`），与 `tests/comprehensive.expected.token` 逐行一致。

### 4.3 单 Token 扫描与最长匹配

保存最后一次到达的接受状态与行列位置，失败时恢复该位置。`a<=b` 识别出完整的 `<=`；`int` 识别成关键字而 `integer` 识别成标识符；`a+++b` 识别为三个独立 `+`。详见 `outputs/longest_match.token`、`outputs/keyword_and_id.token`。

### 4.4 跳过注释及错误恢复

空白和两类注释被过滤，未闭合块注释在起始行列报错；非法字符以标准错误报告并跳过一个字符继续扫描。详见 `outputs/comments.token`、`outputs/invalid_and_continue.stderr.txt`、`outputs/unterminated_comment.stderr.txt`。

## 五、测试结果

自动化测试包括关键字与标识符、数字、最长匹配、注释、非法字符、未闭合注释、空文件、文件末尾无换行、CRLF、负号与综合样例，**11/11 通过**。逐项详情见 `outputs/test_summary.md`。

## 六、实验结论

本实验得到能够在 Linux 下独立编译的完整 ToyC 词法分析器，正确输出指定 Token 格式，并具备源位置记录、注释过滤、最长匹配和基本词法错误恢复功能。综合样例与材料中的完整标准输出逐行一致。

## 七、思考题

**1. `int`、`integer`、`<` 和 `<=` 的最长匹配与优先级如何配合？** 先以最长匹配为准；`integer` 的最长合法匹配是标识符，不能将其中的 `int` 截断成关键字。若同一长度同时匹配关键字和标识符，则依照规则优先级选择关键字。`<=` 比 `<` 长，因此当后面有等号时选择 `<=`。

**2. 非法字符和未闭合注释如何定位并避免死循环？** 扫描时同时维护 `pos`、`line`、`column`；非法字符报告当前坐标并至少推进一个字符；块注释记录起始位置，直到遇到 `*/` 或 EOF。检测到 EOF 且未闭合时报注释起始坐标；扫描位置必须单调推进，因此不会在非法位置无限重试。

## 八、数据核对说明

指导材料正文的“205 Token”与其给出的 `0..207` 不一致，故采用实际的 **208 Token**。`a+++b` 共有 3 个 `+`，产生 5 个 Token，指导材料中“5 个 +”应视为表述错误。
