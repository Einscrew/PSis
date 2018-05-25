#!/bin/bash

sleep 1 && ./app -c $1/clipSocket -s ola -r 0 -i c &
sleep 1 && ./app -c $1/clipSocket -s awd -r 0 -i c &
sleep 1 && ./app -c $1/clipSocket -s a12d -r 0 -i c &
sleep 1 && ./app -c $1/clipSocket -s a100d -r 0 -i c &
sleep 1 && ./app -c $1/clipSocket -s a1d -r 0 -i c &
sleep 1 && ./app -c $1/clipSocket -s a2d -r 0 -i c &
sleep 1 && ./app -c $1/clipSocket -s awed -r 0 -i c &
sleep 1 && ./app -c $1/clipSocket -s awad -r 0 -i c &