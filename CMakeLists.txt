##
# @file CMakeLists.txt
# @brief
# os项目的根
#
# @author Yutengfu
# @email xxf.world@gmail.com
#

project(os)
cmake_minimum_required(VERSION 2.8)
message(STATUS "### Begin to configure project os ###")

# 检测cmake运行操作系统/CPU/编译器
message(STATUS "Generator: ${CMAKE_GENERATOR}")
message(STATUS "Building OS: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Building Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER_ID}")

# 打开vobose选项
set(CMAKE_VERBOSE_MAKEFILE on)

# 编译模式，Release/Debug，-DCMAKE_BUILD_TYPE=Release|Debug
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug) # Debug Release
endif()
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)
if (CMAKE_BUILD_TYPE MATCHES "debug")
    set(CMAKE_BUILD_TYPE Debug)
else()
    set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Building mode: ${CMAKE_BUILD_TYPE}")

# c/c++按照11标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED OFF)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
message(STATUS "C/C++ standard: ${CMAKE_CXX_STANDARD}")

# 设置编译结果输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/bin")


# 生成config.h文件
configure_file(
    ${CMAKE_SOURCE_DIR}/config.h.in
    ${CMAKE_SOURCE_DIR}/include/os/config.h
    NEWLINE_STYLE UNIX
)

# 指定编译子目录
add_subdirectory(src)

message(STATUS "### Done ###")

