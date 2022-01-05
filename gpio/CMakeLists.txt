# Get all source files
file(GLOB GPIO_SRCS "*.c")

# Add executable
add_executable(gpio ${GPIO_SRCS})

# Link dependencies
target_link_libraries(gpio
        wiringPi
        crypt
        pthread
        m
        rt)

# Install to bin/
install (TARGETS gpio
         RUNTIME DESTINATION "bin")