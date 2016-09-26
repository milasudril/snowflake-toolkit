#!/bin/bash

__targets_rel/graupel_generate \
	--prototype=../crystal-prototypes/spheroid-lowpoly.ice \
	--deformations=r_x:1:0,r_y:1:0,r_z:1:0,s:1:0.25 \
	--D_max=40 --fill-ratio=0.7 --dump-geometry=/home/torbjorr/Skrivbord/graupel.obj