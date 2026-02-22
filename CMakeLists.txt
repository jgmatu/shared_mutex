cmake_minimum_required(VERSION 3.10)
project(shared_mutex_project VERSION 1.0 LANGUAGES CXX)

# 1. Compiler Flags (Equivalent to CXXFLAGS)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -O3 -g -Wshadow)

# 2. Locate Threads (Equivalent to -pthread)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# 3. Collect Source Files (Equivalent to SRC_PATH)
# Explicit listing is safer than GLOB for build consistency
file(GLOB SOURCES "src/*.cpp" "src/*.c")

# 4. Define the Executable (Equivalent to TARGET)
add_executable(shared_mutex ${SOURCES})

# 5. Include Directories (Equivalent to INCLUDE)
target_include_directories(shared_mutex PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 6. Link Libraries (Equivalent to LIBFLAGS)
# Links both pthread and the math library (-lm)
target_link_libraries(shared_mutex PRIVATE Threads::Threads m)

# 7. Set Output Path (Equivalent to BIN_PATH)
set_target_properties(shared_mutex PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)