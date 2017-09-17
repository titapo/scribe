#define CATCH_CONFIG_MAIN
#include <tests/catch.hpp>
#include <tests/tree_helpers.h>
#include <scribe/Path.h>

using namespace scribe;

TEST_CASE("checkit")
{
  auto tree = createTreeNode();
  Path::Elements elements;
  SECTION("get leaf of node")
  {
    elements.push_back(std::make_unique<Path::Child>("colors"));
    elements.push_back(std::make_unique<Path::Child>("green"));
    Entity& result = Path{std::move(elements)}.evaluate(*tree);
    REQUIRE_NOTHROW(dynamic_cast<const Leaf<std::string>&>(result));
    REQUIRE_NOTHROW(dynamic_cast<const Leaf<std::string>&>(result).getValue() == "#00ff00");
  }

  SECTION("get array")
  {
    elements.push_back(std::make_unique<Path::Child>("some names"));
    const Entity& result = Path{std::move(elements)}.evaluate(*tree);
    REQUIRE_NOTHROW(dynamic_cast<const Array&>(result));
    REQUIRE_NOTHROW(dynamic_cast<const Array&>(result).size() == 3);
  }

  SECTION("get leaf of array")
  {
    elements.push_back(std::make_unique<Path::Child>("some names"));
    elements.push_back(std::make_unique<Path::Index>(1));
    const Entity& result = Path{std::move(elements)}.evaluate(*tree);
    REQUIRE_NOTHROW(dynamic_cast<const Leaf<std::string>&>(result));
    REQUIRE_NOTHROW(dynamic_cast<const Leaf<std::string>&>(result).getValue() == "Greg");
  }

  SECTION("get nonexisting child of a node")
  {
    elements.push_back(std::make_unique<Path::Child>("foo"));
    REQUIRE_THROWS_AS(Path{std::move(elements)}.evaluate(*tree), NoSuchChild);
  }

}

TEST_CASE("element display")
{
  REQUIRE(Path::Child("lalala").toString() == ".lalala");
  REQUIRE(Path::Index(1234).toString() == "[1234]");
}

TEST_CASE("trace")
{
  auto tree = createTreeNode();
  Path::Elements elements;
  elements.push_back(std::make_unique<Path::Child>("some names"));
  elements.push_back(std::make_unique<Path::Index>(1));
  const PathTrace trace = Path{std::move(elements)}.evaluateTrace(*tree);

  SECTION("check")
  {
    REQUIRE_NOTHROW(dynamic_cast<const Leaf<std::string>&>(trace.getLast()));
    REQUIRE_NOTHROW(dynamic_cast<const Leaf<std::string>&>(trace.getLast()).getValue() == "Greg");
  }

  SECTION("display TODO")
  {
    std::stringstream str;
    for (const auto& step : trace)
      str << step.first;

    REQUIRE(str.str() == ".some names[1]");
  }
}

