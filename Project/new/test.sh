#! /bin/bash
for D in *; do
    if [ -d "${D}" ]; then
    	./app -c "$D"/clipSocket
    	sleep 1
    fi
done
