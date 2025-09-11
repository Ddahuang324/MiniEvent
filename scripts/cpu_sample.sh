#!/bin/bash
# CPU sampling script
echo "Sampling CPU usage..."
# Use `top` on macOS or `ps` on Linux
top -l 1 | grep "CPU usage"
