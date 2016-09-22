#!/bin/bash

for config in *.json; do
	maike --configfiles=alice/maikeconfig.json,"$config"
	if [ $? -ne 0 ]; then
		break;
	fi
done
