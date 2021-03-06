#!/bin/bash

dir_out=/home/torbjorr/snowflake-test

k=22000000
l=1000
m=20000

mkdir -p $dir_out

rm -rf $dir_out/$k-$l-$m
mkdir $dir_out/$k-$l-$m

__targets_rel/snowflake_simulate3 \
	--prototype-choices=prototypeexample.json \
	--output-dir=$dir_out/$k-$l-$m --dump-stats=256 --N=4999 \
	--growthrate=$k --meltrate=$l --droprate=$m \
	--seed=0 --dump-geometry \
	--stop-cond=iterations=262144 \
	--overlap-min=0 \
	--overlap-max=0 \
	--merge-retries=0 


#\
#	| tee $dir_out/$k-$l-$m/params.txt 
