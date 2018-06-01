#! /bin/bash
for D in ../clipboard/*; do
    if [ -d "${D}" ]; then
    	for j in $(seq 0 "$1")
    	do
	        for i in $(seq 0 9)
			do
				r=$((RANDOM % 1000000))
				../app/app -c ../clipboard/"$D"/clipSocket -s "$r" -r "$i" -i c &
			done
		done
    fi
done
