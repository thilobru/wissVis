#pragma once

#include <stdexcept>
#include <string>

namespace StackTrace
{

    /// Creates a backtrace of the current stack
    /// and returns an appropriate string.
    /**
     * In case the backtrace capability is not available
     * on the platform, e.what() is returned to have
     * a basic description of the error.
     *
     * The call to this function and stacksize calls before
     * this call are removed from the list
     */
    std::string getBacktrace( const std::exception& e, size_t stacksize );

    /// Creates a backtrace of the current stack
    /// and returns an appropriate string
    /**
     * In case the backtrace capability is not available
     * on the platform, an empty string is returned
     *
     * The call to this function and stacksize calls before
     * this call are removed from the list
     */
    std::string getBacktrace( size_t stacksize );

} // namespace StackTrace
