cmake_minimum_required(VERSION 3.10)   # 指定最低 CMake 版本

project(MyProject VERSION 1.0)          # 定义项目名称和版本

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON) #强制要求使用指定的 C++ 标准

# 包含头文件路径
include_directories(${PROJECT_SOURCE_DIR}/src/include)
#补：PROJECT_SOURCE_DIR 是 CMake 的一个内置变量，表示当前项目的根目录。这个变量在调用 project() 命令时自动设置，可以用于指定相对于项目根目录的路径。

# 添加子目录
add_subdirectory(src)
add_subdirectory(tests)
