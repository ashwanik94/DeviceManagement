cmake_minimum_required(VERSION 3.16)
project(fleet_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect platform and choose find_package options
if(APPLE)
    message(STATUS "Building on macOS → enabling NO_DEFAULT_PATH for find_package")
    set(FIND_OPTS NO_DEFAULT_PATH)
else()
    message(STATUS "Building on Linux → using default search paths")
    set(FIND_OPTS "")
endif()

# Find dependencies
find_package(Protobuf REQUIRED ${FIND_OPTS})
find_package(gRPC REQUIRED ${FIND_OPTS})

include_directories(${Protobuf_INCLUDE_DIRS} ${gRPC_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/generated)

# generated pb sources
set(PROTO_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/fleet.pb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/fleet.grpc.pb.cc
)

add_executable(fleet_server
    src/main.cpp
    src/fleet_service_impl.cpp
    src/device_management.cpp
    ${PROTO_SRCS}
)

target_include_directories(fleet_server PRIVATE src ${CMAKE_CURRENT_SOURCE_DIR}/generated)

target_link_libraries(fleet_server
    #${Protobuf_LIBRARIES}
    protobuf::libprotobuf
    gRPC::grpc++
    gRPC::grpc++_reflection
)
