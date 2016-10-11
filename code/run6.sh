#!/bin/bash

__targets_rel/graupel_generate \
	--prototype=../crystal-prototypes/spheroid-12.ice \
	--deformations=r_x:1:0,r_y:1:0,r_z:1:0,s:1:0.75 \
	--D_max=60 --fill-ratio=0.0 --dump-geometry=/home/torbjorr/Skrivbord/graupel.obj \
	--E_0=8 --decay_distance=8 --dump-stats=/home/torbjorr/Skrivbord/graupel.txt