PROJECT(spread)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

# silence cmake
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

ADD_DEFINITIONS(-Wall -O2 -g -DNDEBUG)

INCLUDE_DIRECTORIES(include)

ADD_LIBRARY(spreadlib STATIC
            src/buffer
            src/hashers
            src/partition
            src/progress
            src/endpoint
            src/merge
            src/exploder
            )

SET_TARGET_PROPERTIES(spreadlib PROPERTIES OUTPUT_NAME "spread")

ADD_EXECUTABLE(spread
               src/spread_app
               )

ADD_EXECUTABLE(explode
               src/explode_app
               )

TARGET_LINK_LIBRARIES(spread boost_thread boost_system boost_program_options spreadlib)
TARGET_LINK_LIBRARIES(explode boost_thread boost_system boost_program_options spreadlib)

INSTALL(TARGETS spread DESTINATION bin)
INSTALL(TARGETS explode DESTINATION bin)
INSTALL(TARGETS spreadlib ARCHIVE DESTINATION lib)
