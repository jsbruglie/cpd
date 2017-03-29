#!/bin/bash

for directory in $( ls ); do
	if [ -d $directory ]; then
		for file in $( ls $directory ); do
			if [ -x $file ]; then
				rm $directory/*.out
				echo "$directory/$file is executable"
				$directory/$file data/s5e50.in 1 >> $directory/s5e50.1.out
			fi
		done
	fi
done
