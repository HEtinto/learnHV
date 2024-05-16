cmake_minimum_required(VERSION 3.10)

# 设置项目名称
set(PROJECT_NAME tcp_server_simple)
project(${PROJECT_NAME})

# 如果hv库的头文件不在默认搜索路径中，可以添加如下：
include_directories(/usr/local/include/hv)

# 指定hv库的路径（如果不在默认路径中）
link_directories(/usr/local/lib)

# 添加源文件
set(SRC_LIST main.c)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SRC_LIST})

# 链接hv库
target_link_libraries(${PROJECT_NAME} hv)
