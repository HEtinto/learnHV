cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(hv_example)

# 如果hv库的头文件不在默认搜索路径中，可以添加如下：
include_directories(/usr/local/include/hv)

# 指定hv库的路径（如果不在默认路径中）
link_directories(/usr/local/lib)

# 检查hv库是否存在
# find_package(hv REQUIRED)

# 添加源文件
set(SRC_LIST main.c)

# 创建可执行文件
add_executable(hv_example ${SRC_LIST})

# 链接hv库
target_link_libraries(hv_example hv)

# 如果hv库的库文件不在默认搜索路径中，可以添加如下：
# link_directories(/path/to/hv/lib)

# 添加编译标志（如果需要）
# target_compile_options(hv_example PRIVATE -std=c11 -Wall)

# 添加安装规则（如果需要将可执行文件安装到特定目录）
# install(TARGETS hv_example RUNTIME DESTINATION bin)