#!/bin/bash

__targets_rel/graupel_generate2 --seed=123 --E_0=12 --decay-distance=50 --overlap-max=3 \
	--scale=[1,0.25] --merge-offset=0.2 --D_max=40 --fill-ratio=0 --dump-stats=graupel-stats2.txt \
	--statefile-out=2.h5 --dump-geometry-ice=test9.graupel --projection=cylindrical --pmap=unidir.png \
	--beam-width=5


#__targets_rel/graupel_generate2 --seed=$seedval --E_0=0 --decay-distance=40 --overlap-max=3 \
#	--merge-offset=0.2 --D_max=200 --stat --fill-ratio=0 --dump-stats=graupel-output/0-40-1.txt \
#	--dump-geometry-ice=0-40-1.graupel &

#seedval=`od -An -t u4 -N 4 /dev/urandom | sed -e 's/^[ \t]*//'`

#__targets_rel/graupel_generate2 --seed=$seedval --E_0=0 --decay-distance=40 --overlap-max=3 \
#	--merge-offset=0.2 --D_max=200 --stat --fill-ratio=0 --dump-stats=graupel-output/0-40-1.txt \
#	--dump-geometry-ice=0-40-1.graupel &