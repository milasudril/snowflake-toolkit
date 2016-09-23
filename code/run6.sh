#!/bin/bash

__targets_rel/snowflake_simulate4 \
	--prototype=../crystal-prototypes/spheroid-lowpoly.ice \
	--deformations=r_x:1:0,r_y:1:0,r_z:1:0,s:1:0.25 \
	--D_max=100 --dump-geometry=test2.obj
