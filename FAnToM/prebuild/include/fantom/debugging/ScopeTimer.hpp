#pragma once

#include <chrono>
#include <string>


namespace fantom
{
    namespace debugging
    {
        class ScopeTimer
        {
        public:
            using Clock = std::chrono::steady_clock;

            ScopeTimer( std::string message );

            ~ScopeTimer();

        private:
            std::string mMessage;
            Clock::time_point mStartTime;
        };
    } // namespace debugging
} // namespace fantom
