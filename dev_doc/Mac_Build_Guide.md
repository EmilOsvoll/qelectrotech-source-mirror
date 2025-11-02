# QElectroTech macOS Build Guide

This document provides step-by-step instructions for building QElectroTech from source on macOS (Apple Silicon and Intel).

## Prerequisites

### Required Software

1. **Homebrew** - Package manager for macOS

   - Install from [Homebrew Website](https://brew.sh/)
   - Run: `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`

2. **Qt 5** (via Homebrew)

   - Install Qt 5: `brew install qt@5`
   - Note the installation path:
     - Apple Silicon: `/opt/homebrew/opt/qt@5`
     - Intel: `/usr/local/opt/qt@5`

3. **CMake** (via Homebrew)

   - Install: `brew install cmake`
   - Ensure version 3.14 or later

4. **Extra CMake Modules (ECM)** (via Homebrew)

   - Install: `brew install extra-cmake-modules`
   - Required for KDE libraries

5. **Xcode Command Line Tools**

   - Install: `xcode-select --install`
   - Required for C++ compilation

6. **Git**

   - Usually pre-installed, or install via Homebrew: `brew install git`

## Setup Process

### 1. Clone the Repository

```bash
# Clone with submodules
git clone --recursive https://github.com/qelectrotech/qelectrotech-source-mirror.git
cd qelectrotech-source-mirror
```

### 2. Configure Environment Variables

Set up environment variables for Qt and ECM. You can add these to your `~/.zshrc` or `~/.bash_profile`:

**For Apple Silicon:**
```bash
export QTPATH5="$(brew --prefix qt@5)"
export ECMPATH="$(brew --prefix extra-cmake-modules)"
export PATH="$QTPATH5/bin:$PATH"
export CMAKE_PREFIX_PATH="$QTPATH5;$ECMPATH"
```

**For Intel Macs:**
```bash
export QTPATH5="$(brew --prefix qt@5)"
export ECMPATH="$(brew --prefix extra-cmake-modules)"
export PATH="$QTPATH5/bin:$PATH"
export CMAKE_PREFIX_PATH="$QTPATH5;$ECMPATH"
```

Reload your shell configuration:
```bash
source ~/.zshrc  # or source ~/.bash_profile
```

### 3. Configure CMake Build

```bash
# Navigate to project directory
cd qelectrotech-source-mirror

# Create build directory
mkdir -p build
cd build

# Configure CMake
cmake -S .. -B . \
  -DCMAKE_PREFIX_PATH="$QTPATH5;$ECMPATH" \
  -DQt5_DIR="$QTPATH5/lib/cmake/Qt5" \
  -DECM_DIR="$ECMPATH/share/ECM/cmake" \
  -DQT_VERSION_MAJOR=5 \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
  -DQET_MIME_PACKAGE_PATH="share/mime/packages" \
  -DQET_APPDATA_PATH="share/metainfo" \
  -DQET_DESKTOP_PATH="share/applications" \
  -DQET_ICONS_PATH="share/icons/hicolor" \
  -DQET_MAN_PATH="man"
```

### 4. Build the Project

```bash
# Build with CMake
cmake --build . --config Debug

# Or use make directly
make -j$(sysctl -n hw.ncpu)
```

## Troubleshooting Common Issues

### Issue 1: pugixml Not Found

**Error**: `Could not find a package configuration file provided by "pugixml"`

**Solution**: The project should build pugixml from source. Ensure `BUILD_PUGIXML` is set to `ON` in `CMakeLists.txt`:

```cmake
# In CMakeLists.txt, line 51 should be:
set(BUILD_PUGIXML ON)
include(cmake/fetch_pugixml.cmake)
```

If it's set to `OFF`, change it to `ON` to automatically fetch and build pugixml from GitHub.

### Issue 2: MOC Error - Undefined Interface

**Error**: `/sources/titleblock/helpercell.h:30: Error: Undefined interface`

**Solution**: Remove the incorrect `Q_INTERFACES` macro from `helpercell.h`:

```cpp
// Remove this line:
Q_INTERFACES(QGraphicsLayoutItem)

// The class should look like:
class HelperCell : public QGraphicsObject, public QGraphicsLayoutItem {
	Q_OBJECT
	// ... rest of class
```

### Issue 3: Qt Not Found

**Error**: `Could not find a package configuration file provided by "Qt5"`

**Solution**: Ensure Qt 5 is installed and paths are set correctly:

```bash
# Verify Qt installation
brew list qt@5

# Check Qt path
brew --prefix qt@5

# Verify CMake can find Qt
cmake --find-package -DNAME=Qt5 -DCOMPILER_ID=AppleClang -DLANGUAGE=CXX
```

### Issue 4: ECM Not Found

**Error**: `Could not find a package configuration file provided by "ECM"`

**Solution**: Install and configure ECM:

```bash
# Install ECM
brew install extra-cmake-modules

# Verify installation
brew --prefix extra-cmake-modules

# Ensure ECM_DIR is set correctly in CMake command
-DECM_DIR="$(brew --prefix extra-cmake-modules)/share/ECM/cmake"
```

### Issue 5: CMake Deprecation Warnings

**Warning**: `Compatibility with CMake < 3.10 will be removed`

**Solution**: These are warnings from submodules and can be ignored. To suppress, add:

```bash
-DCMAKE_POLICY_VERSION_MINIMUM=3.5
```

### Issue 6: X11 Warning (Apple Silicon)

**Warning**: `Searching for X11 and related technologies is disabled on Apple systems`

**Solution**: This is expected on macOS and can be suppressed:

```bash
-DAPPLE_SUPPRESS_X11_WARNING=ON
```

### Issue 7: Compiler Warnings

**Warning**: Various deprecation and unused variable warnings from KDE libraries

**Solution**: These are warnings from dependencies and don't affect the build. They can be ignored or suppressed with compiler flags if needed.

## Build Output

After successful build, you should find:

- **Main Executable**: `build/src/qelectrotech` (or `build/src/qelectrotech.app`)
- **Libraries**: `build/lib/` containing:
  - Static libraries for dependencies
  - KDE framework libraries

## Running the Application

**Option 1: Run directly from build directory**
```bash
# From build directory
./src/qelectrotech
```

**Option 2: Run as macOS app bundle**
```bash
# If built as .app bundle
open src/qelectrotech.app
```

**Option 3: Install and run**
```bash
# Install to system (optional)
cmake --install . --prefix /usr/local

# Run installed version
qelectrotech
```

## Development Workflow

### Making Changes and Rebuilding

1. **Edit source files** in your preferred editor
2. **Rebuild** using:
   ```bash
   cmake --build . --config Debug
   ```
   Or:
   ```bash
   make -j$(sysctl -n hw.ncpu)
   ```
3. **Test** the changes by running the executable

### Using Xcode

1. **Generate Xcode project**:
   ```bash
   cmake -G Xcode -S .. -B xcode-build
   ```
2. **Open** `xcode-build/qelectrotech.xcodeproj` in Xcode
3. **Build** and **Debug** directly in Xcode

### Using Qt Creator

1. **Install** Qt Creator: `brew install --cask qt-creator`
2. **Open** Qt Creator
3. **File → Open Project...**
4. **Select** the top-level `CMakeLists.txt` in the QElectroTech repo
5. **Configure** using the CMake wizard
6. **Build** using the Build button or Cmd+B

### Using VS Code

1. **Install** CMake Tools extension
2. **Open** the project folder
3. **Configure** CMake to use the build directory
4. **Build** using Cmd+Shift+P → "CMake: Build"

## Environment Variables Summary

For persistent setup, add to `~/.zshrc` (or `~/.bash_profile`):

```bash
# Qt 5 Path
export QTPATH5="$(brew --prefix qt@5)"
export ECMPATH="$(brew --prefix extra-cmake-modules)"

# Add Qt to PATH
export PATH="$QTPATH5/bin:$PATH"

# Set CMAKE_PREFIX_PATH
export CMAKE_PREFIX_PATH="$QTPATH5;$ECMPATH"
```

## Dependencies Summary

| Component         | Version       | Source                   | Purpose              |
| ----------------- | ------------- | ------------------------ | -------------------- |
| Qt                | 5.15.17+      | Homebrew (qt@5)          | GUI Framework        |
| Clang/AppleClang  | 17.0.0+       | Xcode Command Line Tools | C++ Compiler         |
| CMake             | 3.14+         | Homebrew                 | Build System         |
| ECM               | 6.19.0+       | Homebrew                 | Extra CMake Modules  |
| pugixml           | 1.11.4        | Git Fetch                | XML Parsing          |
| KF5CoreAddons     | Latest        | Git Fetch                | KDE Core Libraries   |
| KF5WidgetsAddons  | Latest        | Git Fetch                | KDE Widget Libraries |
| SingleApplication | Latest        | Git Submodule            | Single Instance      |

## Important Notes

- **Qt 5 is required** - The project currently targets Qt 5, not Qt 6
- **Apple Silicon support** - Fully supported, paths may differ from Intel Macs
- **Homebrew paths**:
  - Apple Silicon: `/opt/homebrew/opt/qt@5`
  - Intel: `/usr/local/opt/qt@5`
- **Tests are enabled by default** - Set `PACKAGE_TESTS=OFF` to disable if needed
- **Debug build** is recommended for development
- **Release build** can be created by changing `--config Debug` to `--config Release`
- **Parallel builds** use all CPU cores by default with `-j` flag

## Known Build Warnings

The following warnings are expected and can be safely ignored:

- CMake deprecation warnings from KDE submodules
- X11-related warnings (disabled on macOS by design)
- Deprecation warnings in KDE library code
- Unused variable warnings in third-party dependencies

## Success Indicators

✅ CMake configuration completes without errors  
✅ Build process completes with exit code 0  
✅ `qelectrotech` executable is generated  
✅ All required libraries are built  
✅ Application launches without immediate crashes  
✅ pugixml is automatically fetched and built  

## Quick Reference Commands

```bash
# Full build from scratch
git clone --recursive https://github.com/qelectrotech/qelectrotech-source-mirror.git
cd qelectrotech-source-mirror
QTPATH5="$(brew --prefix qt@5)"
ECMPATH="$(brew --prefix extra-cmake-modules)"
export PATH="$QTPATH5/bin:$PATH"
export CMAKE_PREFIX_PATH="$QTPATH5;$ECMPATH"
mkdir -p build && cd build
cmake -S .. -B . \
  -DCMAKE_PREFIX_PATH="$QTPATH5;$ECMPATH" \
  -DQt5_DIR="$QTPATH5/lib/cmake/Qt5" \
  -DECM_DIR="$ECMPATH/share/ECM/cmake" \
  -DQT_VERSION_MAJOR=5 \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
  -DQET_MIME_PACKAGE_PATH="share/mime/packages" \
  -DQET_APPDATA_PATH="share/metainfo" \
  -DQET_DESKTOP_PATH="share/applications" \
  -DQET_ICONS_PATH="share/icons/hicolor" \
  -DQET_MAN_PATH="man"
cmake --build . --config Debug
```

---

_This guide was created based on successful build on macOS (Darwin 24.6.0) with Qt 5.15.17, CMake 3.14+, and AppleClang 17.0.0._

