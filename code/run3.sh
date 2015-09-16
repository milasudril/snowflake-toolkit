#!/bin/bash

dir_out=/home/torbjorr/snowflakeout-a-2
mkdir -p $dir_out

for a in {0.17,0.22,0.28,0.33,0.39,0.44,0.5,0.55,0.61,0.66}; do
	L=`awk "BEGIN{printf \"%.8g\",0.33*0.33/($a*$a)}"`
	L_std=`awk "BEGIN{printf \"%.8g\",0.25*$L}"`
	rm -rf $dir_out/$a
	mkdir $dir_out/$a
	seedval=`od -An -t u4 -N 4 /dev/urandom | sed -e 's/^[ \t]*//'`
	echo "# Starting simulation for a=$a"
	__wand_targets/snowflake_simulate3 --shape=../crystal-library/bullet.ice \
		--deformation=L,$L,$L_std --deformation=a,$a --deformation=t,0 \
		--output-dir=$dir_out/$a --dump-geometry --dump-stats \
		--growthrate=33000000 --meltrate=500 --droprate=20000 \
		--N=4999 \
		--seed=$seedval \
		--iterations=262144 | tee $dir_out/$a/params.txt
done
