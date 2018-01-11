#ifndef SCRIBE_MAKESTRING_H_INCLUDED
#define SCRIBE_MAKESTRING_H_INCLUDED

#include <sstream>
#include <numeric> // for accumulate

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

  template <typename Range>
  std::string join(const Range& range, const std::string& joiner)
  {
    if (range.begin() == range.end()) // empty
      return {};

    return std::accumulate(range.begin() + 1, range.end(),
        *(range.begin()), [&joiner](const auto& a, const auto& b) -> std::string
        { return makeString() << a << joiner << b; });
  }

}

#endif
