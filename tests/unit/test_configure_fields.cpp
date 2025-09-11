#include "Configure/Configure.hpp"
#include <cassert>
#include <iostream>

// 测试加载有效配置
void test_config_load_valid() {
    // 依赖 T003 创建的 fixture
    MiniEvent::Configure config("../tests/fixtures/example_config.json");
    assert(config.getPort() == 8080);
    assert(config.getTimeout() == 60);
    std::cout << "test_config_load_valid PASSED" << std::endl;
}

// 测试加载缺失文件
void test_config_load_missing_file() {
    bool thrown = false;
    try {
        MiniEvent::Configure config("non_existent_file.json");
    } catch (const std::exception& e) {
        thrown = true;
    }
    assert(thrown);
    std::cout << "test_config_load_missing_file PASSED" << std::endl;
}

int main() {
    test_config_load_valid();
    test_config_load_missing_file();
    // 初始失败占位符
    // assert(false && "Configure test placeholder failed");
    std::cout << "All Configure tests passed (placeholders)." << std::endl;
    return 0;
}
