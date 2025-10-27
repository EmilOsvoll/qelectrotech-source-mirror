# QElectroTech Windows Build Guide

This document provides step-by-step instructions for building QElectroTech from source on Windows 10/11.

## Prerequisites

### Required Software

1. **Visual Studio 2022 Community** (or Professional/Enterprise)

   - Install with "Desktop development with C++" workload
   - Include "CMake tools for C++" workload
   - Ensure MSVC v143 compiler toolset is installed

2. **Qt 6.9.3**

   - Download from [Qt Official Website](https://www.qt.io/download)
   - Install Qt 6.9.3 with MSVC2022 64-bit compiler
   - Note the installation path (typically `C:\Qt\6.9.3\msvc2022_64`)

3. **CMake** (if not included with Visual Studio)

   - Download from [CMake Website](https://cmake.org/download/)
   - Ensure it's added to PATH

4. **Git**
   - Download from [Git Website](https://git-scm.com/download/win)
   - Ensure Git Bash is available

## Setup Process

### 1. Clone the Repository

```bash
# Clone with submodules
git clone --recursive https://github.com/qelectrotech/qelectrotech-source-mirror.git
cd qelectrotech-source-mirror
```

### 2. Install vcpkg Package Manager

```bash
# Navigate to a suitable location (avoid long paths)
cd C:\src

# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

# Bootstrap vcpkg
.\bootstrap-vcpkg.bat

# Install required packages
.\vcpkg install sqlite3:x64-windows
.\vcpkg install pugixml:x64-windows
.\vcpkg install kf5coreaddons:x64-windows
.\vcpkg install kf5widgetsaddons:x64-windows
```

### 3. Configure CMake Build

```bash
# Navigate to project directory
cd C:\Users\emil_\Documents\Repositories\qelectrotech-source-mirror

# Create build directory
mkdir build
cd build

# Configure CMake with vcpkg toolchain
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/src/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_PREFIX_PATH="C:/Qt/6.9.3/msvc2022_64" -DCMAKE_POLICY_VERSION_MINIMUM="3.5" -DPACKAGE_TESTS=OFF
```

### 4. Build the Project

```bash
# Build with CMake
cmake --build . --config Debug
```

## Troubleshooting Common Issues

### Issue 1: CMake Policy Version Error

**Error**: `Invalid CMAKE_POLICY_VERSION_MINIMUM value "3"`

**Solution**: Use quotes around the policy version:

```bash
cmake .. -DCMAKE_POLICY_VERSION_MINIMUM="3.5" [other flags]
```

### Issue 2: Empty String Comparison Error

**Error**: `if given arguments: "STRGREATER" ""`

**Solution**: Fix the comparison in `cmake/define_definitions.cmake`:

```cmake
# Change from:
if(${QET_EXAMPLES_PATH} STRGREATER "")

# To:
if(QET_EXAMPLES_PATH AND QET_EXAMPLES_PATH STRGREATER "")
```

### Issue 3: Missing Windows Path Variables

**Error**: `install DIRECTORY given no DESTINATION!`

**Solution**: Add Windows-specific paths in `cmake/paths_compilation_installation.cmake`:

```cmake
# Add these lines in the Windows section:
set(QET_ICONS_PATH              "icons/")
set(QET_MIME_PACKAGE_PATH       "mime/")
set(QET_DESKTOP_PATH            "applications/")
set(QET_MAN_PATH                "man/")
set(QET_APPDATA_PATH            "appdata/")
set(QET_EXAMPLES_PATH           "examples/")
```

### Issue 4: C++ Compilation Error with std::make_pair

**Error**: `cannot convert argument 1 from 'Terminal *' to '_Ty1 &&'`

**Solution**: Remove explicit template parameters in `sources/undocommand/deleteqgraphicsitemcommand.cpp`:

```cpp
// Change from:
m_connected_terminals.append(std::make_pair<Terminal *, Terminal *>((Terminal *)hub_terminal, (Terminal *)t));

// To:
m_connected_terminals.append(std::make_pair((Terminal *)hub_terminal, (Terminal *)t));
```

### Issue 5: pugixml Target Conflict

**Error**: `Some (but not all) targets in this export set were already defined`

**Solution**: Use system pugixml instead of fetching:

```cmake
# In CMakeLists.txt, replace:
set(BUILD_PUGIXML OFF)
include(cmake/fetch_pugixml.cmake)

# With:
find_package(pugixml REQUIRED)
```

## Build Output

After successful build, you should find:

- **Main Executable**: `build/Debug/qelectrotech.exe` (~24MB)
- **Libraries**: `build/bin/Debug/` containing:
  - `KF5CoreAddons.dll`
  - `KF5WidgetsAddons.dll`
  - `Qt5*.dll` files
  - Various system DLLs

## Running the Application

```bash
# From the build directory
.\Debug\qelectrotech.exe
```

## Development Workflow

### Making Changes and Rebuilding

1. **Edit source files** in your preferred editor
2. **Rebuild** using:
   ```bash
   cmake --build . --config Debug
   ```
3. **Test** the changes by running the executable

### Using Visual Studio

1. **Open** `build/qelectrotech.sln` in Visual Studio
2. **Set** `qelectrotech` as startup project
3. **Build** and **Debug** directly in Visual Studio

### Using VS Code

1. **Install** CMake Tools extension
2. **Open** the project folder
3. **Configure** CMake to use the build directory
4. **Build** using Ctrl+Shift+P → "CMake: Build"

## Environment Variables

For command-line builds, ensure these are set:

```bash
# Add to PATH
C:\Qt\6.9.3\msvc2022_64\bin
C:\Program Files\CMake\bin

# Set CMAKE_PREFIX_PATH
CMAKE_PREFIX_PATH=C:\Qt\6.9.3\msvc2022_64;C:\src\vcpkg\installed\x64-windows
```

## Dependencies Summary

| Component         | Version       | Source                   | Purpose              |
| ----------------- | ------------- | ------------------------ | -------------------- |
| Qt                | 6.9.3         | Qt Installer             | GUI Framework        |
| MSVC              | 19.44.35219.0 | Visual Studio 2022       | C++ Compiler         |
| CMake             | 4.2+          | Visual Studio/Standalone | Build System         |
| vcpkg             | Latest        | Git Clone                | Package Manager      |
| SQLite3           | 3.50.4        | vcpkg                    | Database             |
| pugixml           | 1.15          | vcpkg                    | XML Parsing          |
| KF5CoreAddons     | 5.77.0        | vcpkg                    | KDE Core Libraries   |
| KF5WidgetsAddons  | 5.77.0        | vcpkg                    | KDE Widget Libraries |
| SingleApplication | Latest        | Git Submodule            | Single Instance      |

## Notes

- **Tests are disabled** (`PACKAGE_TESTS=OFF`) to avoid build conflicts
- **Debug build** is recommended for development
- **Release build** can be created by changing `--config Debug` to `--config Release`
- **Long paths** should be avoided (Windows limitation)
- **Git Bash** or **Command Prompt** can be used for commands

## Success Indicators

✅ CMake configuration completes without errors  
✅ Build process completes with exit code 0  
✅ `qelectrotech.exe` is generated (~24MB)  
✅ All required DLLs are present in `bin/Debug/`  
✅ Application launches without immediate crashes

---

_This guide was created based on successful build on Windows 10 build 26100 with Visual Studio 2022 Community and Qt 6.9.3._
