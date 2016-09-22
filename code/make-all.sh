#!/bin/bash

for config in *.json; do
	maike --configfiles=alice/maikeconfig.json,"$config"
done
