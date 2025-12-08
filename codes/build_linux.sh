#!/bin/bash
set -e

echo "========================================"
echo "  Building for Linux (University Env)   "
echo "========================================"

# 1. Clean previous builds
echo "[1/2] Cleaning previous builds..."
make -f Makefile_floor clean_all

# 2. Build the Project
echo "[2/2] Building Aincrad Floor..."
make -f Makefile_floor

echo "========================================"
echo "  Build Complete!                       "
echo "  Run with: ./aincrad_floor             "
echo "========================================"
