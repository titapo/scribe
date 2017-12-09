#define CATCH_CONFIG_MAIN
#include <tests/catch.hpp>

// move to test header
struct ExceptionMessageChecker : public Catch::MatcherBase<std::exception>
{
  std::string expected;

  explicit ExceptionMessageChecker(const std::string& matcher)
    : expected(matcher)
  {}

   bool match(const std::exception& actual) const override
   {
     return expected == actual.what();
   }

  std::string describe() const override
  {
    std::ostringstream ss;
    ss << "matches " << expected;
    return ss.str();
  }
};

template <typename T>
auto WithMessage(T&& t)
{
  return ExceptionMessageChecker(std::forward<T>(t));
}
