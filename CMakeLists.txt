cmake_minimum_required(VERSION 3.0)
project(CommunicationLib)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Find the libserialport library
find_library(SERIALPORT_LIB NAMES serialport)

# Add the source files for the shared library
add_library(libserialcom SHARED libserialcom.c)

# Link the shared library with libserialport
target_link_libraries(libserialcom ${SERIALPORT_LIB})

# Add the executable for the main program
add_executable(main main.c)

# Link the main program with libserialcom
target_link_libraries(main libserialcom)