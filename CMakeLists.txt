cmake_minimum_required(VERSION 3.2)
project(wiringPi LANGUAGES C)

# # Set C++ standard to 14
# set(CMAKE_CXX_STANDARD 14)

# Set compile flags
SET(CMAKE_C_FLAGS "-Wall -Wno-long-long -Wextra -Winline -Wno-format-truncation -pipe -fPIC" CACHE STRING "compile flags" FORCE)
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_C_FLAGS_RELEASE "-O2")

# Add lib
add_subdirectory(wiringPi)

# Add GPIO tool
add_subdirectory(gpio)
