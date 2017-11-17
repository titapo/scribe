#define CATCH_CONFIG_MAIN
#include <tests/catch.hpp>
#include <scribe/Array.h>
#include <scribe/Leaf.h>
#include <scribe/exception.h>

using namespace scribe;

TEST_CASE("empty array")
{
    Array array;
    REQUIRE(array.size() == 0);
}

TEST_CASE("get over-indexed element")
{
    Array array;
    REQUIRE_THROWS_AS(array.getChild(2), NoSuchChild);
    REQUIRE_THROWS_WITH(array.getChild(2), "No element indexed by: 2");
}

TEST_CASE("append element")
{
    Array array;
    array.append(Entity::create<Leaf<int>>(1234));
    REQUIRE(array.size() == 1);
    REQUIRE(dynamic_cast<Leaf<int>&>(array.getChild(0)).getValue() == 1234);
}

TEST_CASE("iterate through const")
{
    const Array array = []
    {
        Array a;
        a.append(Entity::create<Leaf<int>>(1));
        a.append(Entity::create<Leaf<int>>(2));
        a.append(Entity::create<Leaf<int>>(3));
        return a;
    }();
    int sum = 0;
    for (const auto& child : array)
    {
        sum += dynamic_cast<const Leaf<int>&>(child).getValue();
    }
    REQUIRE(array.size() == 3);
}

TEST_CASE("iterate through non-const")
{
    Array array;
    array.append(Entity::create<Leaf<int>>(1));
    array.append(Entity::create<Leaf<int>>(2));
    array.append(Entity::create<Leaf<int>>(3));

    for (auto& child : array)
    {
        auto& leaf = dynamic_cast<Leaf<int>&>(child);
        leaf.setValue(leaf.getValue() - 1);
    }

    REQUIRE(dynamic_cast<Leaf<int>&>(array.getChild(0)).getValue() == 0);
    REQUIRE(dynamic_cast<Leaf<int>&>(array.getChild(1)).getValue() == 1);
    REQUIRE(dynamic_cast<Leaf<int>&>(array.getChild(2)).getValue() == 2);
}
