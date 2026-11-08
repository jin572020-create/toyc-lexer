# 评测入口修复说明

## 经检查确认的问题

GitHub 网页上传的根目录 `compiler` 文件以 Git mode `100644` 保存（不可执行）。旧版 CMake 默认生成 `build/compiler`；而实验指导书展示的运行入口是 `./compiler -token`。若评测器执行根目录 `./compiler`，它会执行失败，造成测试没有合法 Token 输出。

## 修复

1. 在 GitHub 网页删除旧的根目录 `compiler` 二进制文件（该二进制不是源码，且网页上传无法可靠设置 Unix 可执行位）。
2. 使用本修复项目的 `CMakeLists.txt`，它在 `cmake --build build` 时直接生成根目录的可执行 `compiler`。
3. `tests/run_tests.py` 已改成调用根目录 `compiler`。
4. 确保 GitHub 仓库根目录直接包含 `CMakeLists.txt` 和 `src/main.cpp`，没有额外嵌套层级。

## 验证步骤

```bash
cmake -S . -B build
cmake --build build
ls -l ./compiler
./compiler -token < tests/comprehensive.c > actual.token
diff -u tests/comprehensive.expected.token actual.token
python3 tests/run_tests.py
```

编译后根目录 `compiler` 应显示 `-rwx...` 权限，11 项本地测试应通过。

**边界：** 已确认修正了构建入口与执行权限问题，但课程平台内部运行命令和隐藏用例没有提供，不能据此保证修复后的评测一定满分。如平台直接执行 `./compiler` 而不执行 CMake，这需要平台先编译源码；若平台有别的构建命令，应根据它的约定调整。
