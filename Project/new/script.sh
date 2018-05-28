#! /bin/bash
for D in *; do
    if [ -d "${D}" ]; then
    	for j in $(seq 0 1000)
    	do
	        for region in $(seq 0 9)
			do
				r=$((RANDOM % 1000000))
				./app -c "$D"/clipSocket -s "$r" -r "$region" -i c &
			done
		done
    fi
done
