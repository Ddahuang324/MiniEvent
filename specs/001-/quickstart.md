# quickstart.md

## 1. 构建
```
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=17
cmake --build build -j4
```

## 2. 运行冒烟测试 (HTTP)
```
# 启动服务器 (假设生成的可执行: MiniEvent_smoke)
./build/bin/MiniEvent_smoke --port 8080 &
SERVER_PID=$!

sleep 1
curl -s -o /tmp/resp.txt -w "%{http_code}\n" http://127.0.0.1:8080/ > /tmp/code.txt
CODE=$(cat /tmp/code.txt)
if [ "$CODE" = "200" ] && grep -q "Hello" /tmp/resp.txt; then
  echo "SMOKE PASS"
  kill $SERVER_PID
  exit 0
else
  echo "SMOKE FAIL (code=$CODE)"
  kill $SERVER_PID
  exit 1
fi
```

## 3. 运行全部测试
```
ctest --test-dir build -j4 --output-on-failure
```

## 4. 生成覆盖矩阵 (占位)
```
python scripts/gen_coverage_matrix.py > coverage-matrix.md  # (后续实现)
```

## 5. 验证日志格式示例
```
# 期望格式: 2025-09-11T10:05:03Z INFO EventBase - loop started
```

