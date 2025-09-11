#include "Buffer/Buffer.hpp"
#include <cassert>
#include <iostream>

// 简单的测试，验证基本读写
void test_buffer_simple_rw() {
    MiniEvent::Buffer buf;
    std::string data = "hello";
    buf.append(data);
    assert(buf.readableBytes() == 5);
    assert(buf.peekAsString(5) == "hello");
    buf.retrieve(2);
    assert(buf.readableBytes() == 3);
    assert(buf.peekAsString(3) == "llo");
    std::cout << "test_buffer_simple_rw PASSED" << std::endl;
}

// 测试扩容
void test_buffer_expand() {
    MiniEvent::Buffer buf;
    std::string data(1024, 'a');
    buf.append(data);
    assert(buf.readableBytes() == 1024);
    buf.append(data); // 触发扩容
    assert(buf.readableBytes() == 2048);
    std::cout << "test_buffer_expand PASSED" << std::endl;
}

int main() {
    test_buffer_simple_rw();
    test_buffer_expand();
    // 初始失败占位符，实现后移除
    // assert(false && "Buffer test placeholder failed");
    std::cout << "All Buffer tests passed (placeholders)." << std::endl;
    return 0;
}
