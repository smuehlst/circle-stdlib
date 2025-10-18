#!/bin/bash

echo "Waiting for QEMU to open socket 5000..."

while ! nc -z localhost 5000; do   
  sleep 0.1 # wait for 1/10 of the second before check again
done

sleep 10  # ensure listen() is active

echo "Socket test" | nc -v localhost 5000

echo "Socket test completed"
