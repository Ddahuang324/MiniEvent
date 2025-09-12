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

# Option to control building tests (and downloading googletest)
option(BUILD_TESTS "Enable building tests and fetching test deps" ON)
if(BUILD_TESTS)
    # Enable testing and optionally pull in GoogleTest for future unit tests
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.12.1
    )
    # Avoid overriding user's compiler/linker options
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()
endif()

# 设置编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
endif()

if(BUILD_TESTS)
    # Include tests directory to build test/smoke helpers
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
        add_subdirectory(tests)
    endif()
endif()

# ================== Tests ==================
# Test targets and examples (only when BUILD_TESTS is ON)
if(BUILD_TESTS)
    # To integrate GoogleTest:
    # 1. Add FetchContent logic for GoogleTest
    # 2. Use find_package(GTest REQUIRED)
    # 3. Link with GTest::gtest_main
    #
    # Example:
    # FetchContent_Declare(
    #   googletest
    #   GIT_REPOSITORY https://github.com/google/googletest.git
    #   GIT_TAG    release-1.12.1
    # )
    # FetchContent_MakeAvailable(googletest)
    # find_package(GTest REQUIRED)
    # add_executable(MyTests ...)
    # target_link_libraries(MyTests GTest::gtest_main)

    # 添加测试可执行文件，验证库行为
    add_executable(${PROJECT_NAME}_tests
        tests/unit/test_mini_event.cpp
    )
    target_include_directories(${PROJECT_NAME}_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME})

    # 单独的单元测试目标，覆盖轻量单元测试
    add_executable(${PROJECT_NAME}_unit_tests
        tests/unit/test_configure.cpp
    )
    target_include_directories(${PROJECT_NAME}_unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(${PROJECT_NAME}_unit_tests ${PROJECT_NAME})
endif()

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

    # Demo: minimal runtime demo
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/demo/main.cpp")
        add_executable(minievent_demo src/demo/main.cpp)
        target_include_directories(minievent_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
        target_link_libraries(minievent_demo ${PROJECT_NAME})
    endif()

    # Demo: multi-threaded echo server using BufferEvent
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/demo/network_demo.cpp")
        add_executable(minievent_network_demo src/demo/network_demo.cpp)
        target_include_directories(minievent_network_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
        target_link_libraries(minievent_network_demo ${PROJECT_NAME})
    endif()

    # Demo: simple client for echo server
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/demo/client.cpp")
        add_executable(client_demo src/demo/client.cpp)
        target_include_directories(client_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    endif()

    # Demo: http request
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/demo_http_request.cpp")
        add_executable(demo_http_request demo_http_request.cpp)
        target_link_libraries(demo_http_request ${PROJECT_NAME})
        target_include_directories(demo_http_request PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    endif()

    # Smoke 测试：端到端 HTTPServer
    if(BUILD_TESTS)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_smoke_http_server.cpp")
            add_executable(${PROJECT_NAME}_smoke tests/test_smoke_http_server.cpp)
            target_include_directories(${PROJECT_NAME}_smoke PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
            target_link_libraries(${PROJECT_NAME}_smoke ${PROJECT_NAME})
        endif()
    endif()
endif()