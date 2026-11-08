# 步骤3：最长匹配与单个 Token 扫描

## 实现位置
`src/main.cpp` 的 `Lexer::scan()` 中 DFA 转移循环及 `bestRule` / `bestPos` / `bestLine` / `bestCol` 保存、恢复逻辑。

## 执行过程
1. 保存待识别 Token 的起始位置。
2. 从 DFA 初态出发，逐字符转移。
3. 每经过接受状态，更新 `bestRule`、`bestPos` 以及对应行列。
4. 当没有合法转移时，回到最后接受位置并输出最长 Token。
5. 多规则匹配相同长度时，关键字优先于标识符（规则序号小者优先）。

## 结果文件
- `tests/longest_match.c` / `outputs/longest_match.token`
- `tests/keyword_and_id.c` / `outputs/keyword_and_id.token`
- `tests/integer_zero.c` / `outputs/integer_zero.token`
- `tests/negative.c` / `outputs/negative.token`

## 典型输出说明
- `a<=b` → `Ident(a)`, `'<='`, `Ident(b)`，而非 `'<', '='`。
- `a+++b` → `Ident(a)`, `'+'`, `'+'`, `'+'`, `Ident(b)`，共 **5 个 Token、其中 3 个加号**。指导材料表格中的“5 个 +”属于笔误。
- `int integer` → `'int'` 关键字 + `Ident(integer)`；不能把 `integer` 切成 `int` 和 `eger`。
- `012` → `IntConst(0)`, `IntConst(12)`，遵循正则式 `0|[1-9][0-9]*`。
- `-3` → `'-'`, `IntConst(3)`，负号由后续语法分析处理。
