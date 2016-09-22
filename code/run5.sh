#!/bin/bash

dir_out=/home/torbjorr/snowflake-lowdrop

k=22000000
l=1
m=1

mkdir -p $dir_out

rm -rf $dir_out/$k-$l-$m
mkdir $dir_out/$k-$l-$m

__targets_rel/snowflake_simulate3 \
	--shape=../crystal-prototypes/bullet.ice --deformation=L,1,0.25 \
	--deformation=a,0.33 --deformation=t,0 \
	--output-dir=$dir_out/$k-$l-$m --dump-stats=256 --N=4999 \
	--growthrate=$k --meltrate=$l --droprate=$m \
	--seed=0 --dump-geometry \
	--stop-cond=subvols_max=345 \
	--overlap-min=0 \
	--overlap-max=0 \
	--merge-retries=0 \
	| tee $dir_out/$k-$l-$m/params.txt
