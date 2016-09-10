#!/bin/bash

dir_out=/home/torbjorr/temp
k=22000000
l=1000
m=20000

mkdir -p $dir_out

rm -rf $dir_out/$k-$l-$m
mkdir $dir_out/$k-$l-$m

gdb --args __targets_dbg/snowflake_simulate3 \
	--shape=../crystal-prototypes/bullet.ice --deformation=L,1,0.25 \
	--deformation=a,0.33 --deformation=t,0 \
	--output-dir=$dir_out/$k-$l-$m --dump-stats=256 --N=4999 \
	--growthrate=$k --meltrate=$l --droprate=$m \
	--iterations=262144 \
	--seed=0 --dump-geometry \
	| tee $dir_out/$k-$l-$m/params.txt