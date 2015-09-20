#!/usr/bin/bash

for i in $@; do
    echo "Concurrency $i:"
    time ./run.sh bin/release $i 200 200 200 >/dev/null
done

