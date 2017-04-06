#!/bin/bash

FILE_ARRAY=(par_grid par_grid_list par_grid_hash seq_grid seq_grid_list seq_grid_hash)
DIRECTORY_ARRAY=(par_grid par_grid_list par_grid_hash seq_grid seq_grid_list seq_grid_hash)
DATA_ARRAY=(s5e50 s20e400 s50e5k s150e10k s200e50k s500e300k)
GENERATIONS_ARRAY=(10 500 300 1000 1000 2000)
THREADS_ARRAY=(1 2 4 8)

echo "Removing previous results."

mkdir -p results

#TAKE CARE.
rm -f results/*.out
rm -f results/*.csv

echo "Compiling with benchmark option."

make -s clean
make -s benchmark

thread=1

function run {
	counter=0
	echo "Running $1/$2 on data/$3.in for $4 generations with ${OMP_NUM_THREADS} threads."
	while [ $counter -lt 5 ]; do
		counter=$((counter + 1))
		$1/$2 data/$3.in $4 >> results/$3.$4.$2.$5.out
	done
	echo -n "$3.in, $2, $4, $5, " >> results/averages.$2.csv
	awk '{ sum += $1 } END { print sum/NR }' results/$3.$4.$2.$5.out >> results/averages.$2.csv
}

for directory in ${DIRECTORY_ARRAY[@]}; do
	for file in ${FILE_ARRAY[@]}; do
		if [ -f $directory/$file ]; then
			#echo add verification for parallel version. if so, run with 1, 2, 4, 8
			echo "datafile, filename, generations, threads, average_time" >> results/averages.$file.csv
			for (( i = 0 ; i < ${#DATA_ARRAY[@]} ; i=$i+1 ));do
				data_file=${DATA_ARRAY[${i}]}
				if [ -f data/$data_file.in ]; then
					if [[ $file == *"par"* ]]; then
						for thread in ${THREADS_ARRAY[@]}; do	
							export OMP_NUM_THREADS="$thread"
							#echo "Parallel stuff happening for $file with $thread threads."
							run $directory $file $data_file ${GENERATIONS_ARRAY[${i}]} $thread
						done
					else
						export OMP_NUM_THREADS="1"
						#echo "Sequential stuff happening for $file with 1 thread."
						run $directory $file $data_file ${GENERATIONS_ARRAY[${i}]} 1
					fi
				fi
			done
		fi
	done
done

echo "Done! Cleaning Binaries."

make -s clean
