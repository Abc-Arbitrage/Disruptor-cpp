set(script)

function(add_command NAME)
    set(_args "")
    foreach (_arg ${ARGN})
        set(_args "${_args} ${_arg}")
    endforeach()
    set(script "${script}${NAME}(${_args})\n" PARENT_SCOPE)
endfunction()

# List all available tests from executable
execute_process(
    COMMAND "${TEST_EXECUTABLE}" -- --list_tests
    WORKING_DIRECTORY "${TEST_WORKING_DIR}"
    TIMEOUT "${TEST_DISCOVERY_TIMEOUT}"
    OUTPUT_VARIABLE _boost_test_output
    RESULT_VARIABLE _boost_test_result
)

if (NOT ${_boost_test_result} EQUAL 0)
    message(FATAL_ERROR
        "Error listing test content from executable ${TEST_EXECUTABLE}.\n"
        "Result: ${_boost_test_result}\n"
        "Output:\n"
        "    ${_boost_test_output}"
    )
endif()

string(REPLACE "\n" ";" _boost_test_output "${_boost_test_output}")

# Parse output
foreach(line ${_boost_test_output})
    set(test ${line})
    set(pretty_test ${test})
    string(REGEX REPLACE "^MANUAL_" "" pretty_test "${pretty_test}")
    add_command(add_test
        "\"${pretty_test}\""
        "${TEST_EXECUTABLE}"
        "\"--run_test=${line}\""
    )
    if (line MATCHES "^MANUAL_")
        add_command(set_tests_properties
            "${pretty_test}"
            PROPERTIES DISABLED TRUE
        )
    endif()
    add_command(set_tests_properties
        "\"${pretty_test}\""
        PROPERTIES
        WORKING_DIRECTORY "${TEST_WORKING_DIR}"
        LABELS "${TEST_TARGET}"
    )
endforeach()

# Write CTest script
file(WRITE "${CTEST_FILE}" "${script}")
