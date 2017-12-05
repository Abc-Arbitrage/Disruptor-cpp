// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <boost/config.hpp>

#if _MSC_VER // only on Windows

# ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN    // Exclude rarely-used stuff from Windows headers
# endif

# pragma warning(disable: 4512) // Assignment operator could not be generated

# include "targetver.h"
# include <tchar.h>
# include <winsock2.h>
# include <Windows.h>
#endif

// STL includes
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <deque>
#include <exception>
#include <functional>
#include <future>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <vector>