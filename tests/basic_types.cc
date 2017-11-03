#define CATCH_CONFIG_MAIN
#include <tests/catch.hpp>
#include <scribe/Node.h>
#include <scribe/Array.h>
#include <scribe/Leaf.h>
#include <scribe/TypeNotion.h>
#include <scribe/exception.h>

using namespace scribe;

TEST_CASE("any")
{
  types::Any notion;
  SECTION("validate node")
  {
    REQUIRE_NOTHROW(notion.validate(Node()));
  }

  SECTION("validate array")
  {
    REQUIRE_NOTHROW(notion.validate(Array()));
  }

  SECTION("validate leaf")
  {
    REQUIRE_NOTHROW(notion.validate(Leaf<int>(1)));
  }

  SECTION("instantiate")
  {
    REQUIRE_THROWS_AS(notion.instantiate(), ScribeException);
  }
}

TEST_CASE("node")
{
  types::NodeType notion;

  SECTION("validate node")
  {
    REQUIRE_NOTHROW(notion.validate(Node()));
  }

  SECTION("validate array")
  {
    REQUIRE_THROWS_AS(notion.validate(Array()), ScribeException);
  }

  SECTION("validate leaf")
  {
    REQUIRE_THROWS_AS(notion.validate(Leaf<int>(1)), ScribeException);
  }

  SECTION("instantiate")
  {
    REQUIRE(notion.instantiate() != nullptr);
  }

  SECTION("get node")
  {
    REQUIRE_NOTHROW(notion.get(Node()));
  }

  SECTION("get array")
  {
    REQUIRE_THROWS_AS(notion.get(Array()), ScribeException);
  }

  SECTION("get leaf")
  {
    REQUIRE_THROWS_AS(notion.get(Leaf<int>(1)), ScribeException);
  }
}

TEST_CASE("array")
{
  types::ArrayType notion;

  SECTION("validate node")
  {
    REQUIRE_THROWS_AS(notion.validate(Node()), ScribeException);
  }

  SECTION("validate array")
  {
    REQUIRE_NOTHROW(notion.validate(Array()));
  }

  SECTION("validate leaf")
  {
    REQUIRE_THROWS_AS(notion.validate(Leaf<int>(1)), ScribeException);
  }

  SECTION("instantiate")
  {
    REQUIRE_NOTHROW(notion.instantiate() != nullptr);
  }

  SECTION("get node")
  {
    REQUIRE_THROWS_AS(notion.get(Node()), ScribeException);
  }

  SECTION("get array")
  {
    REQUIRE_NOTHROW(notion.get(Array()));
  }

  SECTION("get leaf")
  {
    REQUIRE_THROWS_AS(notion.get(Leaf<int>(1)), ScribeException);
  }
}

TEST_CASE("leaf base")
{
  types::LeafBaseType notion;

  SECTION("validate node")
  {
    REQUIRE_THROWS_AS(notion.validate(Node()), ScribeException);
  }

  SECTION("validate array")
  {
    REQUIRE_THROWS_AS(notion.validate(Array()), ScribeException);
  }

  SECTION("validate leaf")
  {
    REQUIRE_NOTHROW(notion.validate(Leaf<int>(1)));
    REQUIRE_NOTHROW(notion.validate(Leaf<bool>(true)));
  }

  SECTION("instantiate")
  {
    REQUIRE_THROWS_AS(notion.instantiate(), ScribeException);
  }
}

TEST_CASE("leaf")
{
  types::LeafType<int> notion;

  SECTION("validate node")
  {
    REQUIRE_THROWS_AS(notion.validate(Node()), ScribeException);
  }

  SECTION("validate array")
  {
    REQUIRE_THROWS_AS(notion.validate(Array()), ScribeException);
  }

  SECTION("validate leaf")
  {
    REQUIRE_NOTHROW(notion.validate(Leaf<int>(1)));
    REQUIRE_THROWS_AS(notion.validate(Leaf<bool>(true)), ScribeException);
  }

  SECTION("instantiate")
  {
    REQUIRE_THROWS_AS(notion.instantiate(), ScribeException);
  }

  SECTION("get node")
  {
    REQUIRE_THROWS_AS(notion.get(Node()), ScribeException);
  }

  SECTION("get array")
  {
    REQUIRE_THROWS_AS(notion.get(Array()), ScribeException);
  }

  SECTION("get leaf")
  {
    REQUIRE_NOTHROW(notion.get(Leaf<int>(1)));
    REQUIRE_THROWS_AS(notion.get(Leaf<bool>(false)), ScribeException);
  }
}
