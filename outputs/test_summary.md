# ToyC 词法分析器测试报告

自动测试：11/11 通过。

| 测试 | 结果 | Token 数 | 返回码 |
|---|---|---:|---:|
| keyword_and_id | PASS | 4 | 0 |
| integer_zero | PASS | 4 | 0 |
| longest_match | PASS | 13 | 0 |
| comments | PASS | 6 | 0 |
| invalid_and_continue | PASS | 4 | 1 |
| unterminated_comment | PASS | 3 | 1 |
| empty | PASS | 0 | 0 |
| no_final_newline | PASS | 3 | 0 |
| crlf | PASS | 3 | 1 |
| negative | PASS | 6 | 0 |
| comprehensive | PASS | 208 | 0 |

说明：非法字符与未闭合注释将输出到 stderr、返回码为 1；仍保留已识别的合法 Token。
注释、空白本身不生成 Token。
综合样例与指导材料逐行比较，没有差异。
