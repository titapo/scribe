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

namespace
{
  struct NumberRangeIterator
  {
    NumberRangeIterator(int from, int to)
      : begin(from)
      , end(to)
      , current(begin)
    {}

    int next()
    {
      return current++;
    }
    bool hasNext()
    {
      return current != end;
    }

    const int begin;
    const int end;
    int current;
  };
}

SCENARIO("iterator")
{
  TypeRegistry registry;

  registry.registerType("number_range", std::make_unique<types::LeafType<NumberRangeIterator>>());
  registry.registerType("int", std::make_unique<types::LeafType<int>>());

  const auto next = FunctionFactory<std::unique_ptr<Leaf<NumberRangeIterator>>&>()(
      {"next", {TypeName("number_range")}, TypeName("int")},
      [](std::unique_ptr<Leaf<NumberRangeIterator>>& iter)
      { return Entity::create<Leaf<int>>(types::LeafType<NumberRangeIterator>().get(*iter).getValue().next()); });

  REQUIRE(next.isInvokableWith({TypeName("number_range")}, registry));
  REQUIRE(next.results({TypeName("number_range")}, registry).get() == TypeName("int").get());
  const auto number_range = Entity::create<Leaf<NumberRangeIterator>>(1, 3);
  const auto result = next.invoke(registry, {std::move(number_range)});
  REQUIRE(types::LeafType<int>().get(*result).getValue() == 1);

}
