project(Disruptor)
cmake_minimum_required(VERSION 2.6)

find_package(Boost COMPONENTS system thread chrono)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

include_directories("..")


set(Disruptor_sources

    BasicExecutor.cpp
    BlockingWaitStrategy.cpp
    BusySpinWaitStrategy.cpp
    FixedSequenceGroup.cpp
    ProcessingSequenceBarrier.cpp
    ProducerType.cpp
    RoundRobinThreadAffinedTaskScheduler.cpp
    Sequence.cpp
    SequenceGroups.cpp
    SleepingWaitStrategy.cpp
    SpinWait.cpp
    SpinWaitWaitStrategy.cpp
    stdafx.cpp
    ThreadPerTaskScheduler.cpp
    ThreadHelper_Linux.cpp
    ThreadHelper_macOS.cpp
    ThreadHelper_Windows.cpp
    TimeoutBlockingWaitStrategy.cpp
    TypeInfo.cpp
    Util.cpp
    YieldingWaitStrategy.cpp
)

set(Disruptor_headers

    AggregateEventHandler.h
    AlertException.h
    ArgumentException.h
    ArgumentNullException.h
    ArgumentOutOfRangeException.h
    BasicExecutor.h
    BatchEventProcessor.h
    BlockingQueue.h
    BlockingWaitStrategy.h
    BuildConfig.h
    BusySpinWaitStrategy.h
    ClockConfig.h
    ConsumerRepository.h
    Disruptor.h
    EventHandlerGroup.h
    EventPoller.h
    EventProcessorInfo.h
    ExceptionBase.h
    ExceptionHandlerSetting.h
    ExceptionHandlerWrapper.h
    FatalException.h
    FatalExceptionHandler.h
    FixedSequenceGroup.h
    IConsumerInfo.h
    ICursored.h
    IDataProvider.h
    IEventHandler.h
    IEventProcessor.h
    IEventProcessorFactory.h
    IEventReleaseAware.h
    IEventReleaser.h
    IEventSequencer.h
    IEventTranslator.h
    IEventTranslatorVararg.h
    IExceptionHandler.h
    IExecutor.h
    IgnoreExceptionHandler.h
    IHighestPublishedSequenceProvider.h
    ILifecycleAware.h
    InsufficientCapacityException.h
    InvalidOperationException.h
    ISequence.h
    ISequenceBarrier.h
    ISequenced.h
    ISequencer.h
    ISequenceReportingEventHandler.h
    ITaskScheduler.h
    ITimeoutHandler.h
    IWaitStrategy.h
    IWorkHandler.h
    MultiProducerSequencer.h
    NoOpEventProcessor.h
    NotSupportedException.h
    Pragmas.h
    ProcessingSequenceBarrier.h
    ProducerType.h
    RingBuffer.h
    RoundRobinThreadAffinedTaskScheduler.h
    Sequence.h
    SequenceGroups.h
    Sequencer.h
    SingleProducerSequencer.h
    SleepingWaitStrategy.h
    SpinWait.h
    SpinWaitWaitStrategy.h
    ThreadPerTaskScheduler.h
    ThreadHelper.h
    TimeoutBlockingWaitStrategy.h
    TimeoutException.h
    TypeInfo.h
    Util.h
    WorkerPool.h
    WorkerPoolInfo.h
    WorkProcessor.h
    YieldingWaitStrategy.h
)

add_library(DisruptorShared SHARED ${Disruptor_sources})
target_link_libraries(DisruptorShared ${Boost_LIBRARIES})
set_target_properties(DisruptorShared PROPERTIES OUTPUT_NAME Disruptor)
set_target_properties(DisruptorShared PROPERTIES VERSION ${DISRUPTOR_VERSION})
set_target_properties(DisruptorShared PROPERTIES SOVERSION ${DISRUPTOR_VERSION_MAJOR})

add_library(DisruptorStatic STATIC ${Disruptor_sources})
set_target_properties(DisruptorStatic PROPERTIES OUTPUT_NAME Disruptor)

install(FILES ${Disruptor_headers} DESTINATION include/Disruptor)

install(TARGETS DisruptorShared DisruptorStatic
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
