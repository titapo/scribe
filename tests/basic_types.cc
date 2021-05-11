#include <tests/common.h>
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
    REQUIRE_THROWS_MATCHES(notion.instantiate(), ScribeException, WithMessage("'Any' cannot be instantiated!"));
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
    REQUIRE_THROWS_MATCHES(notion.validate(Array()), ScribeException, WithMessage("not a Node"));
  }

  SECTION("validate leaf")
  {
    REQUIRE_THROWS_MATCHES(notion.validate(Leaf<int>(1)), ScribeException, WithMessage("not a Node"));
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
    REQUIRE_THROWS_MATCHES(notion.get(Array()), ScribeException, WithMessage("not a Node"));
  }

  SECTION("get leaf")
  {
    REQUIRE_THROWS_MATCHES(notion.get(Leaf<int>(1)), ScribeException, WithMessage("not a Node"));
  }
}

TEST_CASE("array")
{
  types::ArrayType notion;

  SECTION("validate node")
  {
    REQUIRE_THROWS_MATCHES(notion.validate(Node()), ScribeException, WithMessage("not an Array"));
  }

  SECTION("validate array")
  {
    REQUIRE_NOTHROW(notion.validate(Array()));
  }

  SECTION("validate leaf")
  {
    REQUIRE_THROWS_MATCHES(notion.validate(Leaf<int>(1)), ScribeException, WithMessage("not an Array"));
  }

  SECTION("instantiate")
  {
    REQUIRE_NOTHROW(notion.instantiate() != nullptr);
  }

  SECTION("get node")
  {
    REQUIRE_THROWS_MATCHES(notion.get(Node()), ScribeException, WithMessage("not an Array"));
  }

  SECTION("get array")
  {
    REQUIRE_NOTHROW(notion.get(Array()));
  }

  SECTION("get leaf")
  {
    REQUIRE_THROWS_MATCHES(notion.get(Leaf<int>(1)), ScribeException, WithMessage("not an Array"));
  }
}

TEST_CASE("leaf base")
{
  types::LeafBaseType notion;

  SECTION("validate node")
  {
    REQUIRE_THROWS_MATCHES(notion.validate(Node()), ScribeException, WithMessage("not a Leaf"));
  }

  SECTION("validate array")
  {
    REQUIRE_THROWS_MATCHES(notion.validate(Array()), ScribeException, WithMessage("not a Leaf"));
  }

  SECTION("validate leaf")
  {
    REQUIRE_NOTHROW(notion.validate(Leaf<int>(1)));
    REQUIRE_NOTHROW(notion.validate(Leaf<bool>(true)));
  }

  SECTION("instantiate")
  {
    REQUIRE_THROWS_MATCHES(notion.instantiate(), ScribeException, WithMessage("'Leaf' cannot be instantiated!"));
  }
}

TEST_CASE("leaf")
{
  types::LeafType<int> notion;

  SECTION("validate node")
  {
    REQUIRE_THROWS_MATCHES(notion.validate(Node()), ScribeException, WithMessage("not a Leaf"));
  }

  SECTION("validate array")
  {
    REQUIRE_THROWS_MATCHES(notion.validate(Array()), ScribeException, WithMessage("not a Leaf"));
  }

  SECTION("validate leaf")
  {
    REQUIRE_NOTHROW(notion.validate(Leaf<int>(1)));
    REQUIRE_THROWS_MATCHES(notion.validate(Leaf<bool>(true)), ScribeException, WithMessage("Not a proper leaf!"));
  }

  SECTION("instantiate")
  {
    REQUIRE(notion.instantiate() != nullptr);
  }

  SECTION("get node")
  {
    REQUIRE_THROWS_MATCHES(notion.get(Node()), ScribeException, WithMessage("Not a proper leaf!"));
  }

  SECTION("get array")
  {
    REQUIRE_THROWS_MATCHES(notion.get(Array()), ScribeException, WithMessage("Not a proper leaf!"));
  }

  SECTION("get leaf")
  {
    REQUIRE_NOTHROW(notion.get(Leaf<int>(1)));
    REQUIRE_THROWS_MATCHES(notion.get(Leaf<bool>(false)), ScribeException, WithMessage("Not a proper leaf!"));
  }
}
