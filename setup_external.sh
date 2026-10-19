#!/bin/bash
# setup_external.sh - Download and set up external dependencies

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
EXTERNAL_DIR="$SCRIPT_DIR/external"
mkdir -p "$EXTERNAL_DIR"
cd "$EXTERNAL_DIR"

echo "📦 Setting up external dependencies..."

# ---------- GLM ----------
if [ ! -d "glm" ]; then
    echo "⬇️  Downloading GLM..."
    git clone --depth 1 --branch 0.9.9.8 https://github.com/g-truc/glm.git
else
    echo "✅ GLM already present"
fi

# ---------- GLFW ----------
if [ ! -d "glfw" ]; then
    echo "⬇️  Downloading GLFW..."
    git clone --depth 1 --branch 3.3.9 https://github.com/glfw/glfw.git
    cd glfw
    cmake -S . -B build -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_EXAMPLES=OFF
    cmake --build build --config Release -j$(nproc)
    cd ..
else
    echo "✅ GLFW already present"
fi

# ---------- GLAD ----------
if [ ! -d "glad" ]; then
    echo "⬇️  Downloading GLAD..."
    git clone --depth 1 https://github.com/Dav1dde/glad.git glad-src
    cd glad-src
    # Use the bundled glad generator (Python)
    python3 -m glad --api gl:core=3.3 --out-path=../glad --generator=c --spec=gl
    cd ..
    rm -rf glad-src
else
    echo "✅ GLAD already present"
fi

echo "🎉 External dependencies ready!"
echo ""
echo "Next steps:"
echo "  mkdir build && cd build"
echo "  cmake .."
echo "  make -j\$(nproc)"
echo "  ./SolarSystem"