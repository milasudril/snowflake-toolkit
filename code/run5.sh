#!/bin/bash

dir_out=/home/torbjorr/snowflake-ref

k=22000000
l=1000
m=20000

mkdir -p $dir_out

rm -rf $dir_out/$k-$l-$m
mkdir $dir_out/$k-$l-$m

__targets_rel/snowflake_simulate3 \
	--shape=../crystal-prototypes/bullet.ice --deformation=L,gamma,1,0.25 \
	--deformation=a,dirac,0.33 --deformation=t,dirac,0 \
	--output-dir=$dir_out/$k-$l-$m --dump-stats=256 --N=4999 \
	--growthrate=$k --meltrate=$l --droprate=$m \
	--seed=0 --dump-geometry \
	--stop-cond=iterations=262144 \
	--overlap-min=0 \
	--overlap-max=0 \
	--merge-retries=0 \
	| tee $dir_out/$k-$l-$m/params.txt 
