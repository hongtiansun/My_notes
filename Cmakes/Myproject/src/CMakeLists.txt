# 创建库目标
add_library(MyLib STATIC
    lib/module1.cpp
    lib/module2.cpp
)

# 指定库的头文件
target_include_directories(MyLib PUBLIC ${CMAKE_SOURCE_DIR}/src/include)
#补：CMAKE_SOURCE_DIR表示顶层 CMakeLists.txt 文件所在的目录


# 创建可执行文件目标
add_executable(MyExecutable main.cpp)

# 链接库到可执行文件
target_link_libraries(MyExecutable PRIVATE MyLib)
