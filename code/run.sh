#!/bin/bash

# mkdir -p ~/snowflake-out3

#for k in {22000000,}; do
#	for l in {2000,}; do
#		for m in {20000,50000}; do
#			rm -rf ~/snowflake-out3/$k-$l-$m
#			mkdir ~/snowflake-out3/$k-$l-$m
#			__targets_rel/snowflake_simulate3 \
#				--shape=../crystal-library/bullet.ice --deformation=L --seed=0 \
#				--dump-stats --N=4999 --output-dir=/home/torbjorr/snowflake-out3/$k-$l-$m \
#				--growthrate=$k --meltrate=$l --droprate=$m \
#				--sigma=0.25 --iterations=262144 \
#					2>~/snowflake-out3/$k-$l-$m/errlog.txt \
#					> ~/snowflake-out3/$k-$l-$m/params.txt
#		done
#	done
#done

for k in {33000000,}; do
	for l in {500,1000,2000}; do
		for m in {10000,20000,50000}; do
			rm -rf ~/snowflake-out3/$k-$l-$m
			mkdir ~/snowflake-out3/$k-$l-$m
			__targets_rel/snowflake_simulate3 \
				--shape=../crystal-library/bullet.ice --deformation=L --seed=0 \
				--dump-stats --N=4999 --output-dir=/home/torbjorr/snowflake-out3/$k-$l-$m \
				--growthrate=$k --meltrate=$l --droprate=$m \
				--sigma=0.25 --iterations=262144 \
					2>~/snowflake-out3/$k-$l-$m/errlog.txt \
					> ~/snowflake-out3/$k-$l-$m/params.txt
		done
	done
done
