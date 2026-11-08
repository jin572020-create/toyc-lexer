# 步骤4：空白、注释和词法错误恢复

## 实现位置
`src/main.cpp` 的 `Lexer::skip()`、`Lexer::advance()`、`Lexer::scan()` 错误分支。

## 具体处理
- 空格、Tab 和换行：直接跳过；新行将行号加一、列号重置为 1。
- `//`：读到行尾但不吞掉换行本身。
- `/* ... */`：找到第一个 `*/` 关闭标记；若到文件末尾未闭合，输出**注释开始位置**。
- 非法字符：在 `stderr` 上报告 `line:column`，跳过恰好一个源字符并继续扫描，避免死循环。
- 空文件 / 末尾无换行：正常完成，无额外 Token。
- Windows 风格换行：先将 CRLF 转成 LF，保证位置信息一致。

## 结果文件（配对）
- `tests/comments.c` → `outputs/comments.token`：注释不生成 Token。
- `tests/invalid_and_continue.c` → `outputs/invalid_and_continue.token` 和 `.stderr.txt`：`@` 与 `$` 均报告错误，之后的合法 Token 保留。
- `tests/unterminated_comment.c` → `outputs/unterminated_comment.token` 和 `.stderr.txt`：报告 `2:1: unterminated comment`。
- `tests/empty.c` → `outputs/empty.token`：空输出。
- `tests/no_final_newline.c` → `outputs/no_final_newline.token`：正常扫描。
- `tests/crlf.c` → `outputs/crlf.stderr.txt`：第二行首列错误位置为 `2:1`。

## 实验结果解释
无效字符和未闭合注释属于词法错误，程序在输出标准错误后返回状态码 1；一般情况下，已识别的合法 Token 仍写到 `stdout`。两条输出流分离，符合将 Token 流提供给后续编译器阶段、将诊断提供给开发者的要求。
