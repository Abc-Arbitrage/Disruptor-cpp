// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <boost/config.hpp>

// STL
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <typeinfo>
#include <unordered_map>
#include <vector>

// Boost.Test

#include "Disruptor/Pragmas.h"

DISRUPTOR_PRAGMA_PUSH
DISRUPTOR_PRAGMA_IGNORE_ALL
DISRUPTOR_PRAGMA_IGNORE_UNUSED_VARIABLES

#include <boost/test/results_reporter.hpp>
#include <boost/test/unit_test.hpp>

DISRUPTOR_PRAGMA_POP

// Google Mock
#include <gmock/gmock.h>
