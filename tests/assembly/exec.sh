#!/bin/bash
path_to_sysy_runtime_lib="../../sysy-runtime-lib"
echo "Path to sysy runtime lib: $path_to_sysy_runtime_lib"

mkdir exe_files
for file in *.s
do
    output_asm="${file%.*}.S"
    output_executable="${file%.*}"
    test_input="${file%.*}.txt"
    cd exe_files
    clang -nostdlib -nostdinc -static -target riscv64-unknown-linux-elf -march=rv64im -mabi=lp64 -fuse-ld=lld "$file" -o "$output_asm" -L"$path_to_sysy_runtime_lib" -lsysy
    qemu-riscv64-static "$output_executable" < "$test_input"
    cd ..
done