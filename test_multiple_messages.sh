#!/bin/bash
# 简单的多消息测试

echo "=== 发送多条消息到服务器 ==="

{
    echo "message_1"
    sleep 0.1
    echo "message_2" 
    sleep 0.1
    echo "message_3"
    sleep 0.1
    echo "large_message_$(printf '%0*d' 50 1)"
    sleep 0.1
    echo "final_message"
    sleep 0.1
    echo "exit"
} | ./build/bin/client_demo 127.0.0.1 12345

echo "=== 多消息测试完成，等待连接关闭 ==="
sleep 2  # 给连接关闭留出时间
