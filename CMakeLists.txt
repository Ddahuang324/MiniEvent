cmake_minimum_required(VERSION 3.16)

project(MiniEvent
    VERSION 1.0.0
    DESCRIPTION "A mini event system project"
    LANGUAGES CXX
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 导出编译命令用于IDE支持
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
endif()

# ================== Tests ==================
# 添加测试可执行文件，验证库行为
add_executable(${PROJECT_NAME}_tests
    tests/test_mini_event.cpp
)
target_include_directories(${PROJECT_NAME}_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME})

# 单独的单元测试目标，覆盖轻量单元测试
add_executable(${PROJECT_NAME}_unit_tests
    tests/test_configure.cpp
)
target_include_directories(${PROJECT_NAME}_unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}_unit_tests ${PROJECT_NAME})

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(include)

# 添加源文件（当src目录有文件时）
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

# 如果有源文件，创建库和可执行文件
if(SOURCES)
    # 创建库
    add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
    
    # 设置库的头文件目录
    target_include_directories(${PROJECT_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
    )
    
    # 如果有main.cpp，创建可执行文件
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
        add_executable(${PROJECT_NAME}_app src/main.cpp)
        target_link_libraries(${PROJECT_NAME}_app ${PROJECT_NAME})
    endif()

    # Demo: http request
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/demo_http_request.cpp")
        add_executable(demo_http_request demo_http_request.cpp)
        target_link_libraries(demo_http_request ${PROJECT_NAME})
        target_include_directories(demo_http_request PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    endif()
endif()