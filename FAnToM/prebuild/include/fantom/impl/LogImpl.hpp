#pragma once

#include <iosfwd>

#include "../algorithm.hpp"

namespace fantom
{
    class AlgorithmLogger
    {
    public:
        virtual ~AlgorithmLogger()
        {
        }

        virtual std::ostream& debugLog() const = 0;
        virtual std::ostream& infoLog() const = 0;
        virtual std::ostream& warningLog() const = 0;
        virtual std::ostream& errorLog() const = 0;
        virtual std::unique_ptr< Algorithm::Progress::Impl >
        makeProgress( const Algorithm::Progress& progress ) const = 0;

        virtual bool ask( const std::string& question,
                          const std::string& positiveAnswer,
                          const std::string& negativeAnswer ) const = 0;
    };
}
