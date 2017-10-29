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

      operator std::string() const
      {
        return str.str();
      }

    private:
      std::stringstream str;
  };
}
