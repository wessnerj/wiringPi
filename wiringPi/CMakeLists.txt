# Get all source files
file(GLOB WIRING_PI_SRCS "*.c")
# Get all header files
file(GLOB WIRING_PI_HDRS "*.h")

# Add library
add_library(wiringPi SHARED "${WIRING_PI_SRCS}")
target_include_directories(wiringPi PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
set_target_properties(wiringPi PROPERTIES PUBLIC_HEADER "${WIRING_PI_HDRS}")

# Install to lib/ and include/wiringPi
install (TARGETS wiringPi
         LIBRARY DESTINATION "lib"
         PUBLIC_HEADER DESTINATION "include/wiringPi")