# 1. CMake version requirement
cmake_minimum_required(VERSION 3.10)

# 2. Project name and programming language (without a specific version)
project(CppWebServer LANGUAGES CXX)

# 3. Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 4. Add the include directory so header files can be found
include_directories(${CMAKE_SOURCE_DIR}/include)

# 5. Execute the CMakeLists.txt file inside the src subdirectory
add_subdirectory(src)
