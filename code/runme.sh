__targets_rel/snowflake_simulate3 \
	--shape=../crystal-library/bullet.ice --deformation=L --seed=0 \
	--N=4999 --output-dir=/home/torbjorr/snowflake-out2/15000000-500-10000 \
	--dump-stats \
	--growthrate=15000000 --meltrate=500 --droprate=10000 \
	--sigma=0.25 --iterations=262144 2>~/snowflake-out2/15000000-500-10000/errlog.txt
