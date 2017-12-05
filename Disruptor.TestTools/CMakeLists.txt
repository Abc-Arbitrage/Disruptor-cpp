project(Disruptor.TestTools)
cmake_minimum_required(VERSION 2.6)


find_package(Boost COMPONENTS system)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

include_directories("..")


set(DisruptorTestTools_sources

    CountdownEvent.cpp
    ManualResetEvent.cpp
    ResetEvent.cpp
    ScopeExitFunctor.cpp
    Stopwatch.cpp
    LatencyRecorder.cpp
)

add_library(Disruptor.TestTools STATIC ${DisruptorTestTools_sources})
