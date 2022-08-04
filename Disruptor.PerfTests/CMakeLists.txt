project(Disruptor.PerfTests)
cmake_minimum_required(VERSION 2.6)

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

add_executable(Disruptor.PerfTests ${DisruptorPerfTests_sources})

target_link_libraries(Disruptor.PerfTests
  PRIVATE
    Disruptor
    Disruptor.TestTools
    pthread
)

add_custom_target(performance_test ${CMAKE_CURRENT_BINARY_DIR}/Disruptor.PerfTests)
