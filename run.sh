#!/usr/bin/bash

prog=$1
concurrency=$2

# echo $prog
# echo $concurrency
# echo $h
# echo $w
# echo $runs

mpirun -np $((concurrency+1)) $prog <<<"becool $concurrency
" | grep ' at '

