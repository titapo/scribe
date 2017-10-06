#define CATCH_CONFIG_MAIN
#include <tests/catch.hpp>
#include <scribe/TypeRegistry.h>
#include <scribe/exception.h>

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
    REQUIRE_NOTHROW(type.validate(Leaf<bool>(true)));
  }

}
