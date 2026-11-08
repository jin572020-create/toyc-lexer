# 步骤2：扫描主循环

## 输入与调用
从标准输入读取 ToyC 源码：

```bash
./build/compiler -token < tests/comprehensive.c > outputs/comprehensive.actual.token
```

## 实现位置
`src/main.cpp` 的 `main` 和 `Lexer::scan()`。

1. 读取输入字符流；将 CRLF/CR 归一化为 LF。
2. 从 `pos=0, line=1, column=1` 开始。
3. 重复调用 `skip()` 跳过空白与注释，再用 DFA 从当前位置扫描 Token。
4. 每次找到合法 Token，追加至 `vector<Token>`（保留顺序与重复项）。
5. 遇到 EOF 结束；按照 `序号:类型:内容` 输出到标准输出。

## Token 结构
```cpp
struct Token {
    string kind;       // Ident / IntConst / 'int' / '<=' ...
    string lexeme;     // 源代码中的原始字符
    int line, column;  // 从 1 开始的源代码位置
};
```

## 结果文件
- `tests/comprehensive.c`：从指导材料提取的完整综合输入。
- `tests/comprehensive.expected.token`：指导材料提供的标准 Token 输出。
- `outputs/comprehensive.actual.token`：本程序真实运行输出。
- `outputs/test_summary.md`：测试汇总。

## 实验结果
综合输入产生 208 个 Token，编号从 `0` 到 `207`，与指导材料逐行完全一致。指导材料正文将其描述为“205 个 Token”，与它自身列出的 `0..207` 编号不一致；报告应以实际结果 **208** 为准。
