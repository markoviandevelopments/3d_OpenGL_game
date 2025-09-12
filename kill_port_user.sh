#!/bin/bash

# Script to find and kill processes using port 8042

PORT=8042

# Check if lsof is installed
if ! command -v lsof &> /dev/null; then
    echo "Error: lsof is not installed. Please install it with 'sudo apt-get install lsof'."
    exit 1
fi

# Find processes using port 8042
PIDS=$(lsof -t -i :$PORT)

if [ -z "$PIDS" ]; then
    echo "No process found running on port $PORT."
    exit 0
fi

# Loop through PIDs and kill them
echo "Found processes on port $PORT: $PIDS"
for PID in $PIDS; do
    echo "Killing process $PID..."
    kill -9 $PID
    if [ $? -eq 0 ]; then
        echo "Successfully killed process $PID."
    else
        echo "Failed to kill process $PID."
        exit 1
    fi
done

# Verify port is free
sleep 1 # Give system time to release port
if lsof -i :$PORT &> /dev/null; then
    echo "Error: Port $PORT is still in use after attempting to kill processes."
    exit 1
else
    echo "Port $PORT is now free."
    exit 0
fi
