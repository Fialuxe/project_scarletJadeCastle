#!/bin/bash
set -e

echo "========================================"
echo "  Building for Linux (University Env)   "
echo "========================================"

# 1. Clean previous builds (which might be Mac binaries)
echo "[1/4] Cleaning previous builds..."
make -f Makefile_floor clean
rm -rf libs/assimp/build

# 2. Setup CMake (Download portable binary if missing)
echo "[2/5] Checking for CMake..."
if ! command -v cmake &> /dev/null; then
    echo "CMake not found. Downloading portable CMake..."
    mkdir -p libs/cmake_dist
    cd libs/cmake_dist
    
    # Download CMake 3.28.1 (Linux x86_64)
    if [ ! -f "cmake-3.28.1-linux-x86_64.tar.gz" ]; then
        wget https://github.com/Kitware/CMake/releases/download/v3.28.1/cmake-3.28.1-linux-x86_64.tar.gz
    fi
    
    # Extract
    tar -xzf cmake-3.28.1-linux-x86_64.tar.gz
    
    # Add to PATH
    export PATH=$PWD/cmake-3.28.1-linux-x86_64/bin:$PATH
    cd ../..
    
    echo "CMake installed locally to libs/cmake_dist"
    cmake --version
else
    echo "CMake found: $(cmake --version | head -n 1)"
fi

# 3. Build Assimp Statically
echo "[3/5] Configuring Assimp..."
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

echo "[4/5] Building Assimp (this may take a while)..."
make -j$(nproc)

cd ../../..

# 4. Build the Project
echo "[5/5] Building Aincrad Floor..."
make -f Makefile_floor

echo "========================================"
echo "  Build Complete!                       "
echo "  Run with: ./aincrad_floor             "
echo "========================================"
