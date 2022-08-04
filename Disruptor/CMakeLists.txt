project(Disruptor)
cmake_minimum_required(VERSION 2.6)

if (WITH_CONAN)
  set(Boost_Libraries CONAN_PKG::boost)
else()
  find_package(Boost COMPONENTS system thread chrono REQUIRED)
  set(Boost_Libraries Boost::boost Boost::system Boost::thread Boost::chrono)
endif()

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
    IEventProcessorSequenceAware.h
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

get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

function(configure_disruptor_target DISRUPTOR_TARGET)
  target_include_directories(${DISRUPTOR_TARGET}
    PUBLIC
      $<BUILD_INTERFACE:${PARENT_DIR}>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}
  )
  target_link_libraries(${DISRUPTOR_TARGET} PUBLIC ${Boost_Libraries})
endfunction()


set(DISRUPTOR_TARGETS)

if (DISRUPTOR_BUILD_SHARED)
  message(STATUS
    "Building Disruptor as a shared library."
  )
  add_library(DisruptorShared SHARED ${Disruptor_sources})
  configure_disruptor_target(DisruptorShared)
  
  set_target_properties(DisruptorShared PROPERTIES OUTPUT_NAME Disruptor)
  set_target_properties(DisruptorShared PROPERTIES VERSION ${DISRUPTOR_VERSION})
  set_target_properties(DisruptorShared PROPERTIES SOVERSION ${DISRUPTOR_VERSION_MAJOR})

  list(APPEND DISRUPTOR_TARGETS DisruptorShared)
endif()

if (DISRUPTOR_BUILD_STATIC)
  message(STATUS
    "Building Disruptor as a static library."
  )
  add_library(DisruptorStatic STATIC ${Disruptor_sources})
  configure_disruptor_target(DisruptorStatic)
 
  set_target_properties(DisruptorStatic PROPERTIES OUTPUT_NAME Disruptor)
 
  list(APPEND DISRUPTOR_TARGETS DisruptorStatic)
endif()

if (DISRUPTOR_BUILD_SHARED AND DISRUPTOR_BUILD_STATIC)
  add_library(Disruptor ALIAS DisruptorStatic)
elseif(DISRUPTOR_BUILD_SHARED)
  add_library(Disruptor ALIAS DisruptorShared)
else()
  add_library(Disruptor ALIAS DisruptorStatic)
endif()

install(FILES ${Disruptor_headers} DESTINATION include/Disruptor)

install(TARGETS ${DISRUPTOR_TARGETS}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
