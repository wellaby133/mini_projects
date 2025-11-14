# Quick build and test script for Windows (PowerShell)
# Updated for new folder structure: code/, gtests/, CMake/

# Create build directory
if (!(Test-Path build)) {
    New-Item -ItemType Directory -Name build | Out-Null
}
cd build

# Run CMake pointing to CMake directory
Write-Host "=== Running CMake ===" -ForegroundColor Green
cmake ../CMake

# Build project
Write-Host "=== Building main and tests ===" -ForegroundColor Green
cmake --build . --config Debug

# Run tests
Write-Host "=== Running Google Tests ===" -ForegroundColor Green
./gtests_build/Debug/test_main.exe

Write-Host "=== All tests passed! ===" -ForegroundColor Green
