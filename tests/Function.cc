#include <tests/common.h>
#include <scribe/Function.h>
#include <scribe/Leaf.h>
#include <scribe/TypeNotion.h>
#include <functional>

using namespace scribe;
using namespace Catch::Matchers;

SCENARIO("function -- return")
{
  TypeRegistry registry;
  registry.registerType("int", std::make_unique<types::LeafType<int>>());
  const auto f = FunctionFactory<>()({"f", {}, TypeName("int")}, [](){ return Entity::create<Leaf<int>>(1); });
  REQUIRE(f.isInvokableWith({}, registry));
  REQUIRE(!f.isInvokableWith({TypeName("int")}, registry));
  const auto result = f.invoke(registry, {});
  REQUIRE(types::LeafType<int>().get(*result).getValue() == 1);
}

SCENARIO("function -- get parameter")
{
  TypeRegistry registry;
  registry.registerType("int", std::make_unique<types::LeafType<int>>());
  GIVEN("add function for ints")
  {
    auto wrapped = 
      [](std::unique_ptr<Entity>&& lhs, std::unique_ptr<Entity>&& rhs)
      { return Entity::create<Leaf<int>>(types::LeafType<int>().get(*lhs).getValue() + types::LeafType<int>().get(*rhs).getValue()); };

    const auto f = FunctionFactory<std::unique_ptr<Entity>&&, std::unique_ptr<Entity>&&>()(
        {"add", {TypeName("int"), TypeName("int")}, TypeName("int")},
        std::move(wrapped)
        );

    REQUIRE(!f.isInvokableWith({}, registry));
    REQUIRE(!f.isInvokableWith({TypeName("int")}, registry));
    REQUIRE(f.isInvokableWith({TypeName("int"), TypeName("int")}, registry));
    REQUIRE(!f.isInvokableWith({TypeName("string"), TypeName("string")}, registry));

    Signature::ArgumentList arguments;
    arguments.emplace_back(std::make_unique<Leaf<int>>(2));
    arguments.emplace_back(std::make_unique<Leaf<int>>(3));
    const auto result = f.invoke(registry, std::move(arguments));
    REQUIRE(types::LeafType<int>().get(*result).getValue() == 5);
  }

  GIVEN("add function for strings (not-registered type)")
  {
    auto wrappedString = 
      [](std::unique_ptr<Entity>&& lhs, std::unique_ptr<Entity>&& rhs)
      { return Entity::create<Leaf<std::string>>(types::LeafType<std::string>().get(*lhs).getValue() + types::LeafType<std::string>().get(*rhs).getValue()); };
    const auto g = FunctionFactory<std::unique_ptr<Entity>&&, std::unique_ptr<Entity>&&>()(
        {"add", {TypeName("string"), TypeName("string")}, TypeName("string")},
        std::move(wrappedString)
        );
    REQUIRE(!g.isInvokableWith({TypeName("string"), TypeName("string")}, registry));

    Signature::ArgumentList arguments;
    arguments.emplace_back(std::make_unique<Leaf<std::string>>("a"));
    arguments.emplace_back(std::make_unique<Leaf<std::string>>("b"));
    REQUIRE_THROWS_MATCHES(g.invoke(registry, std::move(arguments)),
        ScribeException, WithMessage("Parameter 0 is invalid! Validation failed! Type 'string' is not registered!"));
  }

}

// TODO check validation of passed parameter types!

SCENARIO("no-operation")
{
  TypeRegistry registry;
  registry.registerType("unit", std::make_unique<types::LeafType<Unit>>());
  const auto f = FunctionFactory<>()({"f", {}, TypeName("unit")}, [](){});
  f.invoke(registry, {}); // ? unit
}

SCENARIO("get result type without call")
{
  TypeRegistry registry;

  const auto f = FunctionFactory<std::unique_ptr<Entity>&&>()(
      {"length", {TypeName("string")}, TypeName("unsigned")},
      [](std::unique_ptr<Entity>&& str)
      { return Entity::create<Leaf<int>>(types::LeafType<std::string>().get(*str).getValue().length()); });

  REQUIRE(!f.isInvokableWith({TypeName("string")}, registry));
  REQUIRE_THROWS_MATCHES(f.results({TypeName("string")}, registry),
      ScribeException, WithMessage("This method is not invokable with [string] !"));

  registry.registerType("string", std::make_unique<types::LeafType<std::string>>());
  REQUIRE(!f.isInvokableWith({TypeName("string")}, registry));
  REQUIRE_THROWS_MATCHES(f.results({TypeName("string")}, registry),
      ScribeException, WithMessage("This method is not invokable with [string] !"));

  registry.registerType("unsigned", std::make_unique<types::LeafType<unsigned>>());
  REQUIRE(f.isInvokableWith({TypeName("string")}, registry));
  REQUIRE(f.results({TypeName("string")}, registry).get() == TypeName("unsigned").get());
}
