#pragma once

#ifdef _MSC_VER

# define DISRUPTOR_PRAGMA_IGNORE_ALL __pragma(warning(push, 0))

# define DISRUPTOR_PRAGMA_PUSH __pragma(warning(push))
# define DISRUPTOR_PRAGMA_POP  __pragma(warning(pop))

# define DISRUPTOR_PRAGMA_IGNORE_CONDITIONAL_EXPRESSION_IS_CONSTANT __pragma(warning(disable : 4127))
# define DISRUPTOR_PRAGMA_IGNORE_UNREACHABLE_CODE                   __pragma(warning(disable : 4702))
# define DISRUPTOR_PRAGMA_IGNORE_DEPRECATED_DECLARATIONS            /* not available with vc++ ? */
# define DISRUPTOR_PRAGMA_IGNORE_UNUSED_VARIABLES                   __pragma(warning(disable : 4189))


#else // ifdef _MSC_VER

# define DISRUPTOR_PRAGMAFY_MESSAGE(text) _Pragma(#text)

# define DISRUPTOR_PRAGMA_IGNORE_ALL                                 /* not available with g++ ? */

# define DISRUPTOR_PRAGMA_PUSH
# define DISRUPTOR_PRAGMA_POP 

# define DISRUPTOR_PRAGMA_IGNORE_CONDITIONAL_EXPRESSION_IS_CONSTANT
# define DISRUPTOR_PRAGMA_IGNORE_UNREACHABLE_CODE
# define DISRUPTOR_PRAGMA_IGNORE_DEPRECATED_DECLARATIONS             DISRUPTOR_PRAGMAFY_MESSAGE(GCC diagnostic ignored "-Wdeprecated-declarations")
# define DISRUPTOR_PRAGMA_IGNORE_UNUSED_VARIABLES                    DISRUPTOR_PRAGMAFY_MESSAGE(GCC diagnostic ignored "-Wunused-variable")

#endif // ifdef _MSC_VER
