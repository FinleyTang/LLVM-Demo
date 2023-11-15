
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
