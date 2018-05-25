#! /bin/bash
for D in *; do
    if [ -d "${D}" ]; then
    	for i in $(seq 0 1)
    	do
	        for i in $(seq 0 9)
			do
				r=$((RANDOM % 1000000))
				./app -c "$D"/clipSocket -s "$r" -r "$i" -i c &
			done
		done
    fi
done
