#!/bin/bash
# 并发客户端测试

echo "=== 并发客户端测试 ==="

# 启动多个客户端进程
launch_client() {
    local client_id=$1
    echo "启动客户端 $client_id..."
    
    {
        echo "hello_from_client_$client_id"
        echo "data_from_client_$client_id"
        echo "exit"
    } | ./build/bin/client_demo 127.0.0.1 12345 &
    
    echo "客户端 $client_id PID: $!"
}

# 启动3个并发客户端
for i in {1..3}; do
    launch_client $i
    sleep 0.2  # 稍微错开启动时间
done

echo "等待所有客户端完成..."
wait

echo "=== 并发客户端测试完成 ==="
