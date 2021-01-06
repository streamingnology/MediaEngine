# CMakeLists.txt
# Copyright (C) orcaer@yeah.net. All rights reserved.
# Last modified: 2019-9-3

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(uv_cpp)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/
		    ../../build/libuv/include)

AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/uv  SOURCES)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/uv/http  SOURCES)

LINK_DIRECTORIES(../../build/libuv/lib/)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11   -O2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -O2")


MESSAGE("building...")

ADD_LIBRARY (uv_cpp SHARED ${SOURCES} )
TARGET_LINK_LIBRARIES(uv_cpp uv -pthread)

ADD_LIBRARY (uv_cpp_a STATIC ${SOURCES} )
TARGET_LINK_LIBRARIES(uv_cpp_a uv_a -pthread)

install(TARGETS uv_cpp uv_cpp_a
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(DIRECTORY uv/include/ DESTINATION include/
        FILES_MATCHING PATTERN "*.hpp")
