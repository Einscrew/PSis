#! /bin/bash
for D in ../clipboard/*; do
    if [ -d "${D}" ]; then
    	../app/app -c ../clipboard/"$D"/clipSocket
    	sleep 1
    fi
done
