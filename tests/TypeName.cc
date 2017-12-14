#include <tests/common.h>
#include <scribe/TypeName.h>

using namespace scribe;
using namespace Catch::Matchers;

TEST_CASE("empty is not allowed")
{
  REQUIRE_FALSE(checkTypeName(""));
}

TEST_CASE("whitespace not allowed")
{
  REQUIRE_FALSE(checkTypeName("a b"));
  REQUIRE_FALSE(checkTypeName("a\tb"));
}


