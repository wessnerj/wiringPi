file(GLOB GPIO_SRCS
        "*.c"
        )
add_executable(gpio ${GPIO_SRCS})
target_link_libraries(gpio
        wiringPi
        crypt
        pthread
        m
        rt)