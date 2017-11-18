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
    REQUIRE_THROWS_WITH(node.getChild("xxx"), "No such child in node: xxx");
}

TEST_CASE("remove nonexisting child")
{
    Node node;
    REQUIRE_NOTHROW(node.removeChild("xxx"));
}

TEST_CASE("add child to node")
{
    Node node;
    node.addChild("name", Entity::create<Leaf<int>>(12));
    REQUIRE(node.size() == 1);
    REQUIRE(dynamic_cast<Leaf<int>&>(node.getChild("name")).getValue() == 12);

    REQUIRE(node.getEntry("name").first == "name");
    REQUIRE(dynamic_cast<Leaf<int>&>(node.getEntry("name").second).getValue() == 12);
}

TEST_CASE("remove child from node")
{
    Node node;
    node.addChild("name", Entity::create<Leaf<int>>(12));
    node.addChild("age", Entity::create<Leaf<int>>(15));
    REQUIRE(node.size() == 2);

    node.removeChild("name");
    REQUIRE(!node.hasChild("name"));
    REQUIRE(node.size() == 1);
}

TEST_CASE("add child as a pair to node")
{
    Node node;
    node.addChild(std::make_pair("name", Entity::create<Leaf<int>>(12)));
}

TEST_CASE("iterating through node")
{
    Node node;
    node.addChild("a", Entity::create<Leaf<int>>(1));
    node.addChild("b", Entity::create<Leaf<int>>(2));
    node.addChild("c", Entity::create<Leaf<int>>(3));
    int sum = 0;
    for (const auto& child : node)
    {
        sum += dynamic_cast<Leaf<int>&>(child.second).getValue();
    }

    REQUIRE(sum == 6);
}

TEST_CASE("iterating through const node")
{
    const auto node = []
    {
        Node n;
        n.addChild("a", Entity::create<Leaf<int>>(1));
        n.addChild("b", Entity::create<Leaf<int>>(2));
        n.addChild("c", Entity::create<Leaf<int>>(3));
        return n;
    }();

    int sum = 0;
    for (const auto& child : node)
    {
        sum += dynamic_cast<Leaf<int>&>(child.second).getValue();
    }

    REQUIRE(sum == 6);
}
