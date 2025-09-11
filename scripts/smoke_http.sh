#!/bin/bash
# Smoke test for HTTP server

PORT=${1:-8080}
HOST="127.0.0.1"
SERVER_BINARY="./build/bin/MiniEvent_smoke" # Adjust if needed

# Start server
$SERVER_BINARY --port $PORT &
SERVER_PID=$!
echo "Server started with PID $SERVER_PID"

sleep 2

# Send request and check response
CODE=$(curl -s -o /dev/null -w "%{http_code}" http://$HOST:$PORT/)

# Clean up
kill $SERVER_PID

if [ "$CODE" = "200" ]; then
  echo "SMOKE TEST PASSED"
  exit 0
else
  echo "SMOKE TEST FAILED (Code: $CODE)"
  exit 1
fi
