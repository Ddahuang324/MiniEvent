#include <cassert>
#include <fstream>
#include <string>
#include <cstdio>

#include "../include/Configure/Configure.hpp"

int main() {
    // 创建临时配置文件
    const char* tmpfile = "test_config.cfg";
    std::ofstream ofs(tmpfile);
    ofs << "# sample config\n";
    ofs << "key1 = value1\n";
    ofs << "key2= value with spaces \n";
    ofs << "#commented=ignored\n";
    ofs.close();

    Configure cfg(tmpfile);

    // 已加载的情况下，get 应返回解析的值
    std::string v1 = cfg.get("key1", "");
    assert(v1 == "value1");

    std::string v2 = cfg.get("key2", "");
    // trim 后应为 "value with spaces"
    assert(v2 == "value with spaces");

    // 不存在的 key 返回默认
    std::string v3 = cfg.get("missing", "def");
    assert(v3 == "def");

    // 清理
    std::remove(tmpfile);

    return 0;
}
