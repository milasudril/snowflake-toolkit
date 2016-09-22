#!/bin/bash

for config in *.json; do
	maike "--configfiles=$config"
done
