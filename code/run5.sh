#!/bin/bash

dir_out=/home/torbjorr/temp3
k=33000000
l=1000
m=20000

mkdir -p $dir_out

rm -rf $dir_out/$k-$l-$m
mkdir $dir_out/$k-$l-$m

__targets_rel/snowflake_simulate3 \
	--shape=../crystal-prototypes/bullet.ice --deformation=L,1,0.25 \
	--deformation=a,0.33 --deformation=t,0 \
	--output-dir=$dir_out/$k-$l-$m --dump-stats=256 --N=4999 \
	--growthrate=$k --meltrate=$l --droprate=$m \
	--seed=0 --dump-geometry \
	--stop-cond=subvols_max=436 \
	--overlap=2 \
	--merge-retries=100000 \
	| tee $dir_out/$k-$l-$m/params.txt
