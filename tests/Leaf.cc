#define CATCH_CONFIG_MAIN
#include <tests/catch.hpp>
#include <scribe/Leaf.h>
#include <scribe/EntityProcessor.h>

using namespace scribe;

TEST_CASE("integer leaf - init")
{
    Leaf<int> leaf(42);
    REQUIRE(leaf.getValue() == 42);
}

TEST_CASE("integer leaf - init const&")
{
    const int value{42};
    Leaf<int> leaf(value);
    REQUIRE(leaf.getValue() == 42);
}

TEST_CASE("integer leaf - setValue")
{
    Leaf<int> leaf(42);
    leaf.setValue(1);
    REQUIRE(leaf.getValue() == 1);
}

TEST_CASE("integer leaf - setValue from other")
{
    Leaf<int> leaf1(42);
    Leaf<int> leaf2(100);
    leaf1.setValue(leaf2.getValue());
    REQUIRE(leaf1.getValue() == 100);
    REQUIRE(leaf2.getValue() == 100);
}

