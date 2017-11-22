#ifndef SCRIBE_MAKESTRING_H_INCLUDED
#define SCRIBE_MAKESTRING_H_INCLUDED

#include <sstream>

namespace scribe
{
  class makeString
  {
    public:

      template <typename T>
      makeString& operator<<(T&& element)
      {
        str << element;
        return *this;
      }

      template <typename T>
      makeString& operator<<(const T& element)
      {
        str << element;
        return *this;
      }

      // google-explicit-constructor is suppressed here, because this is the point now
      operator std::string() const // NOLINT
      {
        return str.str();
      }

    private:
      std::stringstream str;
  };
}

#endif
