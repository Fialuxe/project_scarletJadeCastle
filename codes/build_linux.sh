#!/bin/bash
set -e

# Change to the directory where the script is located
cd "$(dirname "$0")"

# Run make with the correct makefile
make -f Makefile_floor
