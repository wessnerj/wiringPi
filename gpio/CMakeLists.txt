# Get all source files
file(GLOB GPIO_SRCS "*.c")

# Add executable
add_executable(gpio ${GPIO_SRCS})
target_include_directories(gpio PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../wiringPi/")

# Link dependencies
target_link_libraries(gpio
        wiringPi
        crypt
        pthread
        m
        rt)

# Install to bin/
install (TARGETS gpio
         RUNTIME DESTINATION "bin"
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID)