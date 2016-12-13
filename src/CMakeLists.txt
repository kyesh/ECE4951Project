cmake_minimum_required(VERSION 2.8)
project( ECE4951.cpp )
# Locate libraries and headers
find_package(WiringPi REQUIRED)
find_package(Threads REQUIRED) 
find_package( OpenCV REQUIRED )

# Include headers
include_directories(${WIRINGPI_INCLUDE_DIRS}) 

add_executable( ECE4951 ECE4951.cpp )
target_link_libraries( ECE4951 ${OpenCV_LIBS} )

# Link against libraries
target_link_libraries(ECE4951 ${WIRINGPI_LIBRARIES}) 
target_link_libraries(ECE4951 ${CMAKE_THREAD_LIBS_INIT}) 