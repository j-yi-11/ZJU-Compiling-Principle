# AccSys CMake Template

## Build

### Prerequisites

- 支持 C++17 的编译器，例如 clang 或 gcc (推荐 gcc 9.4.0 或更高的版本，有完整 C++ 17 支持)
- flex & bison

#### Install Dependencies

#### Get Template Source

```bash
git clone git@git.zju.edu.cn:accsys/accsys-cmake-template.git
```


### Build

```bash
cmake -B build          # create & generate build configs under `build/` directory
cmake --build build     # build target in `build/` directory
```

#### Adjust Build Options (Optional)

你可以自行修改 `CMakeLists.txt` 并添加编译选项或开关.
下面提供一些示例：

- 为 `clangd` 导出 `compile_commands.json`，可以添加 `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON`.
- 选用不同的构建系统生成器，使用 `-G` 指定，例如使用 `Ninja` 可以添加 `-G Ninja`.

## FAQ

### Why newly added C++ source files are not compiled?

为了方便起见，我们使用 `file(GLOB)` 来指定编译的源代码，你添加了新的源码文件后 `CMakeCache` 并不会更新，构建系统不会意识到更改.
如果遇到这种情况，请重新执行一次 `cmake`.