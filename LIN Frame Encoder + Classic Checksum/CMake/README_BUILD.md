# LIN Frame Encoder + Classic Checksum — Reorganized Structure

## Folder Organization

```
LIN Frame Encoder + Classic Checksum/
├── code/                      # Source code
│   ├── main.c                 # Main application
│   ├── lin_checksum.h         # Header with function declarations
│   ├── lin_checksum.c         # Checksum and parsing functions
│   ├── input_generator.py     # Generate sample inputs.txt
│   └── CMakeLists.txt         # Build config for code
│
├── gtests/                    # Google Test suite
│   ├── test_main.cpp          # 40+ unit and integration tests
│   └── CMakeLists.txt         # Build config for tests
│
├── CMake/                     # CMake build system
│   ├── CMakeLists.txt         # Root CMake configuration
│   ├── build.ps1              # PowerShell build script (Windows)
│   ├── build.sh               # Bash build script (Linux/macOS)
│   └── README_BUILD.md        # Build instructions
│
└── (other project files)
    ├── Diagram/               # Project diagrams
    ├── TEST_README.md         # Test documentation (legacy)
    └── ...
```

## Building and Testing

### On Windows (PowerShell)

```powershell
cd CMake
.\build.ps1
```

### On Linux/macOS (Bash)

```bash
cd CMake
bash build.sh
```

### Manual CMake (any platform)

```bash
mkdir build
cd build
cmake ../CMake
cmake --build . --config Debug
./gtests_build/test_main  # ./gtests_build/Debug/test_main.exe on Windows
```

## Running Just the Main Application

After building:

```powershell
# Windows
.\build\code_build\Debug\main.exe inputs.txt

# Linux/macOS
./build/code_build/main inputs.txt
```

## Test Coverage

**Location:** `gtests/test_main.cpp`  
**Total Tests:** 40+

- **Checksum functions** (8 tests)
  - `lin_checksum_classic()`: empty, single, multiple, overflow, all zeros
  - `lin_checksum_enhanced()`: with PID, empty data, overflow

- **Parsing functions** (22 tests)
  - `parse_hex_byte()`: various formats, error cases
  - `parse_hex_line()`: separators, prefixes, invalid inputs

- **Integration tests** (2+ tests)
  - Full frame parsing + checksum validation

## Dependencies

- **C/C++ Compiler:** GCC or MSVC (via CMake)
- **CMake:** 3.10+
- **Google Test:** Auto-downloaded by CMake

## Next Steps

1. Run the build script to compile and test
2. Verify all 40+ tests pass
3. Modify/extend code in the `code/` folder
4. Add new tests in `gtests/test_main.cpp`
5. Update CMakeLists.txt files if adding new source files
