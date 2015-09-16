#!/bin/bash

dir_out=/home/torbjorr/snowflakeout-rates-1

mkdir -p $dir_out

for k in {15000000,22000000}; do
	for l in {500,1000,2000}; do
		for m in {10000,20000,50000}; do
			rm -rf $dir_out/$k-$l-$m
			mkdir $dir_out/$k-$l-$m
			seedval=`od -An -t u4 -N 4 /dev/urandom | sed -e 's/^[ \t]*//'`
			__wand_targets/snowflake_simulate3 \
				--shape=../crystal-library/bullet.ice --deformation=L,1,0.25 \
				--deformation=a,0.33 --deformation=t,0 \
				--dump-stats --N=4999 --output-dir=$dir_out/$k-$l-$m \
				--growthrate=$k --meltrate=$l --droprate=$m \
				--iterations=262144 \
				--seed=$seedval \
				| tee $dir_out/$k-$l-$m/params.txt
		done
	done
done


#for k in {,33000000}; do
#	for l in {500,1000,2000}; do
#		for m in {10000,20000,50000}; do
#			rm -rf $dir_out/$k-$l-$m
#			mkdir $dir_out/$k-$l-$m
#			seedval=`od -An -t u4 -N 4 /dev/urandom | sed -e 's/^[ \t]*//'`
#			__wand_targets/snowflake_simulate3 \
#				--shape=../crystal-library/bullet.ice --deformation=L,1,0.25 \
#				--deformation=a,0.33 --deformation=t,0 \
#				--dump-stats --N=4999 --output-dir=$dir_out/$k-$l-$m \
#				--growthrate=$k --meltrate=$l --droprate=$m \
#				--iterations=262144 \
#				--seed=$seedval \
#				| tee $dir_out/$k-$l-$m/params.txt
#		done
#	done
#done
