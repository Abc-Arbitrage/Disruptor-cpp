project(Disruptor.PerfTests)
cmake_minimum_required(VERSION 2.6)


find_package(Boost COMPONENTS system thread date_time)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

include_directories("..")


set(DisruptorPerfTests_sources

    main.cpp

    EventCountingQueueProcessor.cpp
    EventCountingWorkHandler.cpp
    FizzBuzzEvent.cpp
    FizzBuzzEventHandler.cpp
    FunctionEvent.cpp
    FunctionEventHandler.cpp
    LatencyTestSession.cpp
    LatencyTestSessionResult.cpp
    LongArrayEventHandler.cpp
    MutableLong.cpp
    OneToOneRawBatchThroughputTest.cpp
    OneToOneRawThroughputTest.cpp
    OneToOneSequencedBatchThroughputTest.cpp
    OneToOneSequencedLongArrayThroughputTest.cpp
    OneToOneSequencedPollerThroughputTest.cpp
    OneToOneSequencedThroughputTest.cpp
    OneToOneTranslatorThroughputTest.cpp
    OneToThreeDiamondSequencedThroughputTest.cpp
    OneToThreePipelineSequencedThroughputTest.cpp
    OneToThreeReleasingWorkerPoolThroughputTest.cpp
    OneToThreeSequencedThroughputTest.cpp
    OneToThreeWorkerPoolThroughputTest.cpp
    Operation.cpp
    PerfTestUtil.cpp
    PingPongSequencedLatencyTest.cpp
    TestRepository.cpp
    ThreeToOneSequencedBatchThroughputTest.cpp
    ThreeToOneSequencedThroughputTest.cpp
    ThreeToThreeSequencedThroughputTest.cpp
    ThroughputTestSession.cpp
    ThroughputTestSessionResult.cpp
    TwoToTwoWorkProcessorThroughputTest.cpp
    ValueAdditionEventHandler.cpp
    ValueEvent.cpp
    ValueMutationEventHandler.cpp
)

add_definitions(-DBOOST_TEST_DYN_LINK)

add_executable(Disruptor.PerfTests ${DisruptorPerfTests_sources})
target_link_libraries(Disruptor.PerfTests DisruptorStatic Disruptor.TestTools ${Boost_LIBRARIES} pthread)

add_custom_target(performance_test ${CMAKE_CURRENT_BINARY_DIR}/Disruptor.PerfTests)
