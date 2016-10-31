#!/bin/bash

for k in {1..4}; do
	sleep 1
	seedval=`od -An -t u4 -N 4 /dev/urandom | sed -e 's/^[ \t]*//'`
	cmd="__targets_rel/graupel_generate2 --seed=$seedval --E_0=12 --decay-distance=30 --overlap-max=3 \
		--scale=[1,0.25] --merge-offset=0.2 --D_max=200 --fill-ratio=0 --dump-stats=graupel-output/12-30-$k.txt \
		--dump-geometry-ice=graupel-output/12-30-$k.graupel > graupel-output/12-30-params-$k.txt;bash"
	screen -dm bash -c "$cmd"
done


#__targets_rel/graupel_generate2 --seed=$seedval --E_0=0 --decay-distance=40 --overlap-max=3 \
#	--merge-offset=0.2 --D_max=200 --stat --fill-ratio=0 --dump-stats=graupel-output/0-40-1.txt \
#	--dump-geometry-ice=0-40-1.graupel &

#seedval=`od -An -t u4 -N 4 /dev/urandom | sed -e 's/^[ \t]*//'`

#__targets_rel/graupel_generate2 --seed=$seedval --E_0=0 --decay-distance=40 --overlap-max=3 \
#	--merge-offset=0.2 --D_max=200 --stat --fill-ratio=0 --dump-stats=graupel-output/0-40-1.txt \
#	--dump-geometry-ice=0-40-1.graupel &