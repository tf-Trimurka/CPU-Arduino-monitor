#!/bin/bash

PORT="/dev/ttyUSB0"
BAUD="9600"
while true; do
    stty -F $PORT $BAUD
    cpu=$(mpstat 1 1 | awk 'NR==4 {printf "%.0f", 100 - $12}')
    ram=$(free | grep Mem | awk '{printf "%.0f", $3/$2 * 100}')
    echo "CPU:${cpu}%,RAM:${ram}%" > $PORT 
    echo "Sent: CPU:${cpu}%,RAM:${ram}%"
    sleep 2
done
