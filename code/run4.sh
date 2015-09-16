#!/bin/bash

dir_out=/home/torbjorr/snowflakeout-rates-1

mkdir -p $dir_out
a=0.22
L=`awk "BEGIN{printf \"%.8g\",0.33*0.33/($a*$a)}"`
L_std=`awk "BEGIN{printf \"%.8g\",0.25*$L}"`

echo $a
echo $L
echo $L_std
dir_out="/home/torbjorr/snowflakeout-other"
mkdir -p $dir_out

for k in {44000000,}; do
	for l in {2000,}; do
		for m in {500,}; do
			rm -rf $dir_out/$k-$l-$m-$a
			mkdir $dir_out/$k-$l-$m-$a
			seedval=`od -An -t u4 -N 4 /dev/urandom | sed -e 's/^[ \t]*//'`
			__wand_targets/snowflake_simulate3 \
				--shape=../crystal-library/bullet.ice --deformation=L,$L,$L_std \
				--deformation=a,$a --deformation=t,0 \
				--dump-geometry --dump-stats --output-dir=$dir_out/$k-$l-$m \
				--N=4999 --growthrate=$k --meltrate=$l --droprate=$m \
				--iterations=2097152 \
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
