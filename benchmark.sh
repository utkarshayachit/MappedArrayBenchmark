#!/bin/bash

for i in 256 512 1024 2048 4096
do
    ./Magnitude $i &>> mag.dat
done

for i in 256 512 1024 2048 4096
do
    ./Gradient $i &>> grad.dat
done
