project(Disruptor.Tests)
cmake_minimum_required(VERSION 2.6)

if (WITH_CONAN)
  set(GMock_LIBRARY CONAN_PKG::gtest)
  set(Boost_Libraries CONAN_PKG::boost)
else()
  set(GMock_LIBRARY gmock)
  find_package(Boost COMPONENTS unit_test_framework REQUIRED)
  set(Boost_Libraries Boost::unit_test_framwork)
endif()

set(DisruptorTests_sources

    main.cpp
    
    AggregateEventHandlerTests.cpp
    AggregateEventHandlerTestsFixture.cpp
    BatchEventProcessorTests.cpp
    BatchEventProcessorTestsFixture.cpp
    BatchingTests.cpp
    BusySpinWaitStrategyTests.cpp
    ConsumerRepositoryTests.cpp
    ConsumerRepositoryTestsFixture.cpp
    DelayedEventHandler.cpp
    DisruptorFixture.cpp
    DisruptorStressTest.cpp
    DisruptorTests.cpp
    DummySequenceBarrier.cpp
    EventPollerTests.cpp
    EventPublisherTests.cpp
    ExceptionThrowingEventHandler.cpp
    FatalExceptionHandlerTests.cpp
    FixedSequenceGroupTest.cpp
    IgnoreExceptionHandlerTests.cpp
    LifecycleAwareTests.cpp
    MultiProducerSequencerTests.cpp
    RingBufferTests.cpp
    RingBufferTestsFixture.cpp
    RingBufferWithMocksTest.cpp
    SequenceBarrierTests.cpp
    SequenceBarrierTestsFixture.cpp
    SequenceReportingCallbackTests.cpp
    SequencerTests.cpp
    SequenceUpdater.cpp
    SleepingEventHandler.cpp
    SleepingWaitStrategyTests.cpp
    SpinWaitWaitStrategyTests.cpp
    StubEvent.cpp
    StubExecutor.cpp
    StubPublisher.cpp
    TestWaiter.cpp
    TestWorkHandler.cpp
    TimeoutBlockingWaitStrategyTests.cpp
    UtilTests.cpp
    WaitStrategyTestUtil.cpp
    WorkerPoolTests.cpp
    YieldingWaitStrategyTests.cpp
)

add_executable(Disruptor.Tests ${DisruptorTests_sources})
target_link_libraries(Disruptor.Tests
  PRIVATE
    Disruptor.TestTools
    ${GMock_LIBRARY}
    ${Boost_LIBRARIES}
)

add_test(cmake_Disruptor.Tests ${CMAKE_CURRENT_BINARY_DIR}/Disruptor.Tests --result_code=no --report_level=no)
