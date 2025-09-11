#include <iostream>
#include <cstdlib>
#include <cassert>

// 这个测试文件将简单地调用 smoke_http.sh 脚本
// 脚本的退出码将决定测试是否通过

int main() {
    std::cout << "Running smoke test script..." << std::endl;
    int result = system("../scripts/smoke_http.sh 8088"); // 使用一个不常用的端口
    
    if (result == 0) {
        std::cout << "SMOKE TEST SCRIPT PASSED" << std::endl;
    } else {
        std::cerr << "SMOKE TEST SCRIPT FAILED with exit code " << result << std::endl;
    }
    
    // assert(result == 0 && "Smoke test script failed");
    
    return result == 0 ? 0 : 1;
}
