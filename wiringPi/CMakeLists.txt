file(GLOB WIRING_PI_SRCS
  "*.c"
  )
add_library(wiringPi SHARED ${WIRING_PI_SRCS})
