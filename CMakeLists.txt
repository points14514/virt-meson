 
name: Build Virt-Meson

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y cmake libgtk-3-dev
        
    - name: Create build directory
      run: mkdir build
      
    - name: Configure with CMake
      working-directory: ./build
      run: cmake ..
      
    - name: Build
      working-directory: ./build
      run: make
      
    - name: Run tests (可选)
      working-directory: ./build
      run: ./virt-meson --version
 
