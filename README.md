
## LLVM环境安装

```
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
sudo apt-add-repository "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-17 main"
sudo apt-get update
sudo apt-get install -y llvm-17 llvm-17-dev llvm-17-tools clang-17
```

## 运行Demo

```
export LLVM_DIR=<installation/dir/of/llvm/17>
mkdir build
cd build
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR <source/dir/llvm/tutor>/HelloWorld/
make
```

这里我们是直接用apt-get install安装的，默认路径是/usr/lib/llvm-17
设置：` export LLVM_DIR=/usr/lib/llvm-17`
执行：
```
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ../HelloWorld/
make 
clang-17 -O1 -S -emit-llvm /home/ubuntu/mylab/llvm-tutor/inputs/input_for_hello.c -o input_for_hello.ll
opt-17 -load-pass-plugin ./libHelloWorld.so -passes=hello-world -disable-output input_for_hello.ll
```

注意 ：

new pass manager with opt:
```
opt-17 -load-pass-plugin ./libHelloWorld.so -passes=hello-world -disable-output input_for_hello.ll
```

legacy pass manager with opt:
```angular2html
opt-17 -load-pass-plugin ./libHelloWorld.so -passes=hello-world -disable-output input_for_hello.ll

```


## OpcodeCounter
OpcodeCounter 类是一个 FunctionAnalysis，它负责实际对函数进行分析并生成相应的结果。在 LLVM 中，FunctionAnalysis OpcodeCounter 类实现了对函数中所有指令的操作码进行提取，并返回对应的字符串结果。

OpcodeCounterPrinter 类是一个 Pass，它将 MyAnalysis 包装成一个可被 LLVM 工具链调用的 pass。Pass 是 LLVM 中用于表示各种转换、分析和优化的基本单元，可以在 LLVM 的优化流水线中组合使用。OpcodeCounterPrinter 类中的 run 函数负责打印分析结果，并返回表示分析没有修改 LLVM IR 代码的标记。

因此，两个类的设计是符合 LLVM 框架中分析和优化的一般模式的：一个类负责实际的分析过程，另一个类负责将该分析包装成一个可被 LLVM 工具链调用的 pass。这样的设计使得分析部分和 pass 部分的职责清晰，易于维护和组合使用。
