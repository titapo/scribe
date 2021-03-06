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
    REQUIRE_THROWS_MATCHES(registry.getType("no"), ScribeException, WithMessage("Type 'no' is not registered!"));
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
    REQUIRE_THROWS_MATCHES(registry.registerType("bool", std::make_unique<types::LeafType<bool>>()),
        ScribeException, WithMessage("'bool' is already registered!"));
  }

  SECTION("register and validate type")
  {
    registry.registerType("bool", std::make_unique<types::LeafType<bool>>());
    REQUIRE_NOTHROW(registry.validate(*Entity::create<Leaf<bool>>(true)));
    REQUIRE_THROWS_MATCHES(registry.validate(*Entity::create<Leaf<int>>(1)),
        ScribeException, WithMessage("Validation failed for: 1"));
  }

  SECTION("register and validate more types")
  {
    registry.registerType("bool", std::make_unique<types::LeafType<bool>>());
    registry.registerType("node", std::make_unique<types::NodeType>());
    REQUIRE_NOTHROW(registry.validate(*Entity::create<Leaf<bool>>(true)));
    REQUIRE_NOTHROW(registry.validate(*Entity::create<Node>()));
  }

}

// TODO scrictness/lazy
TEST_CASE("expected type")
{
  TypeRegistry registry;
  registry.registerType("bool", std::make_unique<types::LeafType<bool>>());
  registry.registerType("node", std::make_unique<types::NodeType>());

  REQUIRE_THROWS_MATCHES(registry.validate(*Entity::create<Leaf<bool>>(true), ValidationContext(TypeName("node"))),
      TypeValidationError, WithMessage("Validation failed! Expected 'node'! Reason: 'not a Node', Got: 'true'"));

  //registry.validate(*Entity::create<Leaf<bool>>(true), ValidationContext(TypeName("xxx")));
  REQUIRE_THROWS_MATCHES(registry.validate(*Entity::create<Leaf<bool>>(true), ValidationContext(TypeName("xxx"))),
      TypeValidationError, WithMessage("Validation failed! Type 'xxx' is not registered!"));
}

TEST_CASE("supporting generics")
{

  TypeRegistry registry;
  auto def = std::unique_ptr<meta::GenericTypeDefinition>(new meta::GenericTypeDefinition(TypeName("MyType"), {"A", "B", "C"}));
  def->addField({"alpha", TypeName("A")});
  registry.registerGeneric(std::move(def));

  SECTION("specialize for unregistered")
  {
    REQUIRE_THROWS_MATCHES(registry.getSpecializedType(TypeName("notKnown"), {}),
        ScribeException, WithMessage("Generic 'notKnown' is not registered!"));
  }

  SECTION("specialize for unknown types")
  {
    REQUIRE_THROWS_MATCHES(registry.getSpecializedType(TypeName("MyType"), {TypeName("a"), TypeName("b"), TypeName("c")}),
        ScribeException, WithMessage("Type 'a' is not registered!"));
  }

  SECTION("specialize")
  {
    registry.registerType("bool", std::make_unique<types::LeafType<bool>>());
    const auto& concrete = registry.getSpecializedType(TypeName("MyType"), {TypeName("bool"), TypeName("b"), TypeName("c")});
    Node node;
    meta::TypeReference("MyType<bool, b, c>").addToNode(node);
    node.addChild("alpha", Entity::create<Leaf<bool>>(true));
    concrete.validate(node, {});
  }
}
