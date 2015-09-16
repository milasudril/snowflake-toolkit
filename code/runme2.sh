#!/bin/bash

#this one is good!

set -x

rm -rf ~/snowflake-out-10000000-1000-20000
konsole --hold -e __wand_targets/snowflake_simulate3 \
--shape=../crystal-library/bullet.ice --deformation=L --seed=0 \
--dump-stats --N=4999 --output-dir=/home/torbjorr/snowflake-out-10000000-1000-20000 \
--growthrate=10000000 --meltrate=1000 --droprate=20000 \
--dump-geometry --sigma=0.25 --iterations=65536 &

rm -rf ~/snowflake-out-15000000-1000-20000
konsole --hold -e __wand_targets/snowflake_simulate3 \
--shape=../crystal-library/bullet.ice --deformation=L --seed=0 \
--dump-stats --N=4999 --output-dir=/home/torbjorr/snowflake-out-15000000-1000-20000 \
--growthrate=15000000 --meltrate=1000 --droprate=20000 \
--dump-geometry --sigma=0.25 --iterations=65536 &

rm -rf ~/snowflake-out-20000000-1000-20000
konsole --hold -e __wand_targets/snowflake_simulate3 \
--shape=../crystal-library/bullet.ice --deformation=L --seed=0 \
--dump-stats --N=4999 --output-dir=/home/torbjorr/snowflake-out-20000000-1000-20000 \
--growthrate=20000000 --meltrate=1000 --droprate=20000 \
--dump-geometry --sigma=0.25 --iterations=65536 &

rm -rf ~/snowflake-out-25000000-1000-20000
konsole --hold -e __wand_targets/snowflake_simulate3 \
--shape=../crystal-library/bullet.ice --deformation=L --seed=0 \
--dump-stats --N=4999 --output-dir=/home/torbjorr/snowflake-out-25000000-1000-20000 \
--growthrate=25000000 --meltrate=1000 --droprate=20000 \
--dump-geometry --sigma=0.25 --iterations=65536 &
