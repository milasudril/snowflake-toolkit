rm -rf ~/snowflake-out4 && __wand_targets/snowflake_simulate3 \
--shape=../crystal-library/bullet.ice --deformation=L --seed=0 \
--stat-output=/home/torbjorr/snowflake-out4 --N=4999 \
--growthrate=24000000 --meltrate=500 --droprate=30000 \
--mesh-output=/home/torbjorr/snowflake-out4 --sigma=0.25