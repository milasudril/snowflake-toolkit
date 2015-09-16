#!/bin/bash

for x in ../aggregates/*.flake; do
	filename=$(basename "$x")
	echo Generating ${filename%.*}
	__wand_targets/snowflake_generate --aggregate=$x\
		--mesh-output=../output/"${filename%.*}".obj\
		--sample-geometry=16,16,16,../output/"${filename%.*}".adda
done
