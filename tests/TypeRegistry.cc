#include <tests/common.h>
#include <scribe/TypeRegistry.h>
#include <scribe/RegisterableTypeNotion.h>
#include <scribe/exception.h>
#include <scribe/Node.h>

using namespace scribe;

TEST_CASE("check")
{
  TypeRegistry registry;
  SECTION("get nonexistent type")
  {
    REQUIRE_THROWS_AS(registry.getType("no"), ScribeException);
  }

  SECTION("register and get type")
  {
    registry.registerType("bool", std::make_unique<types::LeafType<bool>>());
    const auto& type = registry.getType("bool");
    REQUIRE_NOTHROW(type.validate(Leaf<bool>(true), {})); // This is not a good practice, call validate via TypeRegistry
  }
  
  SECTION("register type with the same name")
  {
    registry.registerType("bool", std::make_unique<types::LeafType<bool>>());
    REQUIRE_THROWS_AS(registry.registerType("bool", std::make_unique<types::LeafType<bool>>()), ScribeException);
    REQUIRE_THROWS_WITH(registry.registerType("bool", std::make_unique<types::LeafType<bool>>()), "'bool' is already registered!");
  }

  SECTION("register and validate type")
  {
    registry.registerType("bool", std::make_unique<types::LeafType<bool>>());
    REQUIRE_NOTHROW(registry.validate(*Entity::create<Leaf<bool>>(true)));
    REQUIRE_THROWS_AS(registry.validate(*Entity::create<Leaf<int>>(1)), ScribeException);
    REQUIRE_THROWS_WITH(registry.validate(*Entity::create<Leaf<int>>(1)), "Validation failed for: 1");
  }

  SECTION("register and validate more types")
  {
    registry.registerType("bool", std::make_unique<types::LeafType<bool>>());
    registry.registerType("node", std::make_unique<types::NodeType>());
    REQUIRE_NOTHROW(registry.validate(*Entity::create<Leaf<bool>>(true)));
    REQUIRE_NOTHROW(registry.validate(*Entity::create<Node>()));
  }

}
