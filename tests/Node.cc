#define CATCH_CONFIG_MAIN
#include <tests/catch.hpp>
#include <scribe/Node.h>
#include <scribe/Leaf.h>
#include <scribe/EntityProcessor.h>

using namespace scribe;

TEST_CASE("empty node")
{
    Node node;
    REQUIRE(node.size() == 0);
}

TEST_CASE("get nonexisting child")
{
    Node node;
    REQUIRE_THROWS_AS(node.getChild("xxx"), NoSuchChild);
    REQUIRE_THROWS_WITH(node.getChild("xxx"), "No such child");
}

TEST_CASE("add child to node")
{
    Node node;
    node.addChild("name", std::unique_ptr<Entity>(new Leaf<int>(12)));
    REQUIRE(node.size() == 1);
    REQUIRE(dynamic_cast<Leaf<int>&>(node.getChild("name")).getValue() == 12);
}

