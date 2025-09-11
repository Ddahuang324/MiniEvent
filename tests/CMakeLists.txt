
# Build a small smoke executable that verifies connection timeouts
add_executable(timeout_smoke smoke/test_timeout_smoke.cpp)
target_include_directories(timeout_smoke PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(timeout_smoke MiniEvent)
add_test(NAME timeout_smoke COMMAND timeout_smoke)

add_executable(connection_manager_cleanup component/test_connection_manager_cleanup.cpp)
target_include_directories(connection_manager_cleanup PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(connection_manager_cleanup MiniEvent)
add_test(NAME connection_manager_cleanup COMMAND connection_manager_cleanup)

add_executable(connection_manager_concurrency component/test_connection_manager_concurrency.cpp)
target_include_directories(connection_manager_concurrency PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(connection_manager_concurrency MiniEvent pthread)
add_test(NAME connection_manager_concurrency COMMAND connection_manager_concurrency)

# Example: a tiny GoogleTest to demonstrate integration (kept optional)
add_executable(example_gtest unit/example_gtest.cpp)
target_link_libraries(example_gtest GTest::gtest_main MiniEvent)
add_test(NAME example_gtest COMMAND example_gtest)

