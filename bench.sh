#!/usr/bin/bash

for i in $@; do
    echo "Concurrency $i:"
    bash ./run.sh bin/release $i
done

