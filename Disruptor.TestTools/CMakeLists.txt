project(Disruptor.TestTools)
cmake_minimum_required(VERSION 2.6)

set(DisruptorTestTools_sources

    CountdownEvent.cpp
    ManualResetEvent.cpp
    ResetEvent.cpp
    ScopeExitFunctor.cpp
    Stopwatch.cpp
    LatencyRecorder.cpp
)

add_library(Disruptor.TestTools STATIC ${DisruptorTestTools_sources})

target_link_libraries(Disruptor.TestTools
  PUBLIC
    Disruptor
)
