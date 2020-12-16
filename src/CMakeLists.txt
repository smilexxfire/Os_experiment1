##
# @file CMakeLists.txt
# @brief
# src目录的CMakeLists.txt
#
# @author Yutengfu
# @email xxf.world@gmail.com
#

include_directories(${CMAKE_SOURCE_DIR}/include/ ${CMAKE_SOURCE_DIR}/src)

aux_source_directory(./os DIR_LIB_SRCS)

# 生成链接库
add_library (Myos ${DIR_LIB_SRCS})

# 指定生成目标
add_executable(os main.cc)

# 添加链接库
target_link_libraries(os Myos)
