#!/bin/bash
# 通信测试脚本

echo "=== MiniEvent 通信测试 ==="

# 检查服务器是否运行
if ! pgrep -f "minievent_network_demo" > /dev/null; then
    echo "错误：服务器未运行，请先启动 ./build/bin/minievent_network_demo"
    exit 1
fi

echo "1. 测试持续通信（发送多条消息）"
echo "-----------------------------------"

# 创建测试客户端脚本
cat > /tmp/test_client.sh << 'EOF'
#!/bin/bash
CLIENT_ID=$1
PORT=${2:-12345}

echo "客户端 $CLIENT_ID 开始测试..."

# 使用 expect 或者直接用管道发送多条消息
{
    echo "hello_from_client_$CLIENT_ID"
    sleep 0.1
    echo "message_2_from_client_$CLIENT_ID"
    sleep 0.1
    echo "message_3_from_client_$CLIENT_ID"
    sleep 0.1
    echo "large_data_$(printf '%0*d' 100 $CLIENT_ID)"  # 100字符的数据
    sleep 0.1
    echo "exit"
} | timeout 10 ./build/bin/client_demo 127.0.0.1 $PORT

echo "客户端 $CLIENT_ID 测试完成"
EOF

chmod +x /tmp/test_client.sh

# 测试1: 持续通信
echo "启动单客户端持续通信测试..."
/tmp/test_client.sh 1 &
CLIENT1_PID=$!

sleep 3
echo ""

# 测试2: 并发客户端
echo "2. 测试并发客户端连接"
echo "-----------------------------------"
echo "启动3个并发客户端..."

for i in {2..4}; do
    /tmp/test_client.sh $i &
    echo "客户端 $i 已启动"
    sleep 0.5  # 错开启动时间
done

# 等待所有客户端完成
wait $CLIENT1_PID
wait

echo ""
echo "3. 测试大数据传输"
echo "-----------------------------------"

# 创建大数据测试
cat > /tmp/large_data_test.sh << 'EOF'
#!/bin/bash
echo "发送大数据块测试..."

# 生成1KB的数据
LARGE_DATA=$(printf '%0*d' 1024 123)

{
    echo "$LARGE_DATA"
    sleep 0.2
    echo "exit"
} | timeout 10 ./build/bin/client_demo 127.0.0.1 12345

echo "大数据测试完成"
EOF

chmod +x /tmp/large_data_test.sh
/tmp/large_data_test.sh

echo ""
echo "=== 测试完成 ==="
echo "请检查服务端日志，确认："
echo "- 所有连接都被正确接受"
echo "- 所有消息都被正确回显" 
echo "- 连接正确关闭"
echo "- 无崩溃或错误"

# 清理
rm -f /tmp/test_client.sh /tmp/large_data_test.sh
