#include "include/Sever/HTTPRequest.hpp"
#include "include/EventBase.hpp"
#include "include/MiniEventLog.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== HTTP Request Demo ===" << std::endl;
    
    // 初始化事件循环
    EventBase loop;
    
    // 创建HTTP请求对象
    auto httpReq = std::make_shared<HttpRequest>(&loop);
    
    // 设置回调函数
    user_callback callback = [&loop](E_RESPONSE_TYPE type, Buffer* buffer) {
        switch (type) {
            case RESPONSE_OK:
                std::cout << "✅ HTTP Request Success!" << std::endl;
                if (buffer && buffer->readableBytes() > 0) {
                    std::string response(buffer->peek(), buffer->readableBytes());
                    std::cout << "📊 Response size: " << response.size() << " bytes" << std::endl;
                    // 打印前300个字符
                    if (response.size() > 300) {
                        std::cout << "📄 Response preview: " << response.substr(0, 300) << "..." << std::endl;
                    } else {
                        std::cout << "📄 Full Response: " << response << std::endl;
                    }
                }
                break;
            case CONNECTION_EXCEPTION:
                std::cout << "❌ Connection Exception!" << std::endl;
                break;
            case REQUEST_TIMEOUT:
                std::cout << "⏰ Request Timeout!" << std::endl;
                break;
            case RESPONSE_ERROR:
                std::cout << "⚠️ Response Error!" << std::endl;
                break;
        }
        loop.quit();
    };
    
    // 发起HTTP POST请求到测试服务器
    std::string url = "http://httpbin.org/post";
    std::string postData = "name=MiniEvent&type=HttpClient&version=1.0";
    
    std::cout << "🚀 Sending HTTP POST request to: " << url << std::endl;
    std::cout << "📤 POST data: " << postData << std::endl;
    
    // 发送请求
    httpReq->postHttpRequest(url, postData, callback);
    
    // 在单独的线程中运行事件循环
    std::cout << "🔄 Starting event loop..." << std::endl;
    std::thread eventThread([&loop]() {
        loop.loop();
    });
    
    // 等待完成
    eventThread.join();
    
    std::cout << "✨ HTTP request demo completed." << std::endl;
    
    return 0;
}
