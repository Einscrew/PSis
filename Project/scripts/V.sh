#!/bin/bash
cd ../clipboard

if [ "$#" -eq 2 ]; then
	./clipboard -c $1 $2 &
else
	./clipboard &
fi
sleep 2
./clipboard -c 127.0.0.1 8000 &
./clipboard -c 127.0.0.1 8000 &
sleep 2
./clipboard -c 127.0.0.1 8001 &
./clipboard -c 127.0.0.1 8002 &
sleep 2
./clipboard -c 127.0.0.1 8003 &
./clipboard -c 127.0.0.1 8004 &
sleep 2
./clipboard -c 127.0.0.1 8005 &
./clipboard -c 127.0.0.1 8006 &
sleep 2
./clipboard -c 127.0.0.1 8007 &
./clipboard -c 127.0.0.1 8008 &
 sleep 2