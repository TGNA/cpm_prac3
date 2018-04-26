#!/bin/bash

make build_parallel

if [ $? -ne 0 ]; then
    echo "Compilation failed ☠️"
    exit 1
fi

echo

run () {
	TPAR=$( { gtime -f "%e" mpirun -c $2 -npernode $3 ./prac_p $1 > /dev/null; } 2>&1 )
	echo "C: $2 N: $3 Time: $TPAR"
}

run_iterations() {
	for i in $(seq 1 10) 
	do
		echo -n "$1 $i: "
		run $1 $2 $3
	done
}

run_configurations() {
	run_iterations $1 2 1
	run_iterations $1 4 1
	run_iterations $1 8 1
	run_iterations $1 4 2
	run_iterations $1 8 2
	run_iterations $1 16 2
	run_iterations $1 12 3
	run_iterations $1 24 3
	run_iterations $1 32 4
	run_iterations $1 64 4
	run_iterations $1 128 4
}

run_configurations 2000
run_configurations 3000
run_configurations 4000
