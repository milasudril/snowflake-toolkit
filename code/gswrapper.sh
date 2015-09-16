#!/bin/bash

ARGS=()
ARGS+=("-dGraphicsAlphaBits=1")
for var in "$@"; do
	[ "$var" != '-dGraphicsAlphaBits=4' ] && ARGS+=("$var")
done
gs "${ARGS[@]}"
