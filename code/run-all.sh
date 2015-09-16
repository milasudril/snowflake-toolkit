#!/bin/bash

set -x

konsole --hold -e ./run.sh 10000000 &
konsole --hold -e ./run.sh 15000000 &
konsole --hold -e ./run.sh 20000000 &
konsole --hold -e ./run.sh 25000000 &
