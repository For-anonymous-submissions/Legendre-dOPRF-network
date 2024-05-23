#!/bin/bash
make

# Define server amounts
IDS=(0 1 2 3)

# Start the servers
for ID in "${IDS[@]}"; do
    echo "Starting server on port $ID"
    ./server128 $ID > "server128_${ID}.log" 2>&1 &
    SERVER_PIDS+=($!)
done

# Give servers a moment to start up
sleep 2

# Start the client
echo "Starting client"
./client128 > client128.log 2>&1

# Wait for client to finish
CLIENT_PID=$!
wait $CLIENT_PID

# Stop the servers
# for PID in "${SERVER_PIDS[@]}"; do
#     echo "Stopping server with PID $PID"
#     kill $PID
# done

echo "All processes completed. Logs are available in server128_*.log and client128.log."