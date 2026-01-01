#!/bin/bash

echo "Waiting for QEMU to open socket 5000..."

sleep 10  # ensure listen() is active

echo "Socket test" | nc -v localhost 5000

echo "Socket test completed"
