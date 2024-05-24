cmake_minimum_required(VERSION 3.10)

# 设置项目名称
set(PROJECT_NAME native_chat_app_service)
project(${PROJECT_NAME})

# 添加源文件
set(SRC_LIST main.c)

# 添加调试信息
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SRC_LIST})

# 链接hv库
# target_link_libraries(${PROJECT_NAME} key)