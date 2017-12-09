#include <tests/common.h>
#include <scribe/PathFormat.h>
#include <scribe/exception.h>

using namespace scribe;

TEST_CASE("BasicPathFormat")
{
  BasicPathFormat format;

  SECTION("parse empty")
  {
    REQUIRE_THROWS_AS(format.parseElement(""), ScribeException);
  }

  SECTION("parse childelement from string")
  {
    const auto result = format.parseElement(".abc");
    REQUIRE(result->toString() == ".abc");
  }

  SECTION("parse indexelement from string")
  {
    const auto result = format.parseElement("[123]");
    REQUIRE(result->toString() == "[123]");
  }

}

TEST_CASE("parse path")
{
  BasicPathFormat format;
  const auto result = format.parse(".abc[123].cde[2][3]");
  REQUIRE(result.toString() == ".abc[123].cde[2][3]");
}

