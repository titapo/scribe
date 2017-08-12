#ifndef SCRIBE_EXCEPTION_H_INCLUDED
#define SCRIBE_EXCEPTION_H_INCLUDED

#include <stdexcept>

namespace scribe
{
    class ScribeException : public std::runtime_error
    {
        public:
            using std::runtime_error::runtime_error;
    };
}
#endif
