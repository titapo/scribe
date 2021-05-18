#include "common.h"
#include "tree_helpers.h"
#include <scribe/Path.h>

using namespace scribe;

TEST_CASE("checkit")
{
  auto tree = createTreeNode();
  Path::Elements elements;
  SECTION("get leaf of node")
  {
    elements.push_back(std::make_unique<Child>("colors"));
    elements.push_back(std::make_unique<Child>("green"));
    Entity& result = Path{std::move(elements)}.evaluate(*tree);
    REQUIRE_NOTHROW(dynamic_cast<const Leaf<std::string>&>(result));
    REQUIRE_NOTHROW(dynamic_cast<const Leaf<std::string>&>(result).getValue() == "#00ff00");
  }

  SECTION("get array")
  {
    elements.push_back(std::make_unique<Child>("some names"));
    const Entity& result = Path{std::move(elements)}.evaluate(*tree);
    REQUIRE_NOTHROW(dynamic_cast<const Array&>(result));
    REQUIRE_NOTHROW(dynamic_cast<const Array&>(result).size() == 3);
  }

  SECTION("get leaf of array")
  {
    elements.push_back(std::make_unique<Child>("some names"));
    elements.push_back(std::make_unique<Index>(1));
    const Entity& result = Path{std::move(elements)}.evaluate(*tree);
    REQUIRE_NOTHROW(dynamic_cast<const Leaf<std::string>&>(result));
    REQUIRE_NOTHROW(dynamic_cast<const Leaf<std::string>&>(result).getValue() == "Greg");
  }

  SECTION("get nonexisting child of a node")
  {
    elements.push_back(std::make_unique<Child>("foo"));
    REQUIRE_THROWS_MATCHES(Path{std::move(elements)}.evaluate(*tree), NoSuchChild, WithMessage("Cannot access child! No such child in node: foo"));
  }
}

TEST_CASE("element display")
{
  REQUIRE(Child("lalala").toString() == ".lalala");
  REQUIRE(Index(1234).toString() == "[1234]");
}

TEST_CASE("path display")
{
  Path::Elements elements;
  elements.push_back(std::make_unique<Child>("colors"));
  elements.push_back(std::make_unique<Child>("green"));
  elements.push_back(std::make_unique<Index>(5));
  const Path path{std::move(elements)};
  REQUIRE(path.toString() == ".colors.green[5]");
}

TEST_CASE("trace")
{
  auto tree = createTreeNode();
  Path::Elements elements;
  elements.push_back(std::make_unique<Child>("some names"));
  elements.push_back(std::make_unique<Index>(1));
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

