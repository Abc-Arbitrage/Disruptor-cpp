function(boost_test_discover_tests TARGET)
    cmake_parse_arguments(
        BOOST_TEST
        ""
        "WORKING_DIRECTORY;DISCOVERY_TIMEOUT"
        "EXTRA_ARGS"
        ${ARGN}
    )
  if (NOT BOOST_TEST_WORKING_DIRECTORY)
      set(BOOST_TEST_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
  endif()
  if (NOT BOOST_TEST_DISCOVERY_TIMEOUT)
      set(BOOST_TEST_DISCOVERY_TIMEOUT 10)
  endif() 
  get_property(
    has_counter
    TARGET ${TARGET}
    PROPERTY BOOST_TEST_DISCOVERED_TEST_COUNTER
    SET
  )
  if(has_counter)
    get_property(
      counter
      TARGET ${TARGET}
      PROPERTY BOOST_TEST_DISCOVERED_TEST_COUNTER
    )
    math(EXPR counter "${counter} + 1")
  else()
    set(counter 1)
  endif()
  set_property(
    TARGET ${TARGET}
    PROPERTY BOOST_TEST_DISCOVERED_TEST_COUNTER
    ${counter}
  )

  # Define rule to generate test list for aforementioned test executable
  set(ctest_file_base "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_${counter}")
  set(ctest_include_file "${ctest_file_base}_include.cmake")
  set(ctest_tests_file "${ctest_file_base}_tests.cmake")
  add_custom_command(
      TARGET ${TARGET} POST_BUILD
      COMMAND "${CMAKE_COMMAND}"
              -D "TEST_TARGET=${TARGET}"
              -D "TEST_EXECUTABLE=$<TARGET_FILE:${TARGET}>"
              -D "TEST_WORKING_DIR=${BOOST_TEST_WORKING_DIRECTORY}"
              -D "TEST_DISCOVERY_TIMEOUT=${BOOST_TEST_DISCOVERY_TIMEOUT}"
              -D "CTEST_FILE=${ctest_tests_file}"
              -P "${_BOOST_TEST_DISCOVER_TEST_SCRIPTS}"
      VERBATIM
  )

  file(WRITE "${ctest_include_file}"
    "if(EXISTS \"${ctest_tests_file}\")\n"
    "  include(\"${ctest_tests_file}\")\n"
    "else()\n"
    "  add_test(${TARGET}_NOT_BUILT ${TARGET}_NOT_BUILT)\n"
    "endif()\n"
  )

  # Add discovered tests to directory TEST_INCLUDE_FILES
  set_property(DIRECTORY
    APPEND PROPERTY TEST_INCLUDE_FILES "${ctest_include_file}"
  )

endfunction()

set(_BOOST_TEST_DISCOVER_TEST_SCRIPTS
    ${CMAKE_CURRENT_LIST_DIR}/BoostTestDiscoverTests.cmake
)
