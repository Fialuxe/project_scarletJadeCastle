#!/bin/bash
set -e

echo "========================================"
echo "  Building for Linux (University Env)   "
echo "========================================"

# 1. Clean previous builds (which might be Mac binaries)
echo "[1/4] Cleaning previous builds..."
make -f Makefile_floor clean
rm -rf libs/assimp/build

# 2. Build Assimp Statically
echo "[2/4] Configuring Assimp..."
mkdir -p libs/assimp/build
cd libs/assimp/build

# Configure with CMake (Disable unnecessary features to speed up build)
cmake .. \
    -DBUILD_SHARED_LIBS=OFF \
    -DASSIMP_BUILD_TESTS=OFF \
    -DASSIMP_BUILD_ASSIMP_TOOLS=OFF \
    -DASSIMP_NO_EXPORT=ON \
    -DASSIMP_BUILD_ZLIB=ON \
    -DCMAKE_BUILD_TYPE=Release

echo "[3/4] Building Assimp (this may take a while)..."
make -j$(nproc)

cd ../../..

# 3. Build the Project
echo "[4/4] Building Aincrad Floor..."
make -f Makefile_floor

echo "========================================"
echo "  Build Complete!                       "
echo "  Run with: ./aincrad_floor             "
echo "========================================"
