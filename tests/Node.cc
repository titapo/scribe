#include <tests/common.h>
#include <scribe/Node.h>
#include <scribe/Leaf.h>
#include <scribe/EntityProcessor.h>
#include <scribe/TypeNotion.h>

using namespace scribe;

SCENARIO("empty node")
{
  GIVEN("a node")
  {
    Node node;
    WHEN("in initial state")
    {
      THEN("it's size is zero")
      {
        REQUIRE(node.size() == 0);
      }

      THEN("it has no child")
      {
        REQUIRE_THROWS_MATCHES(node.getChild("xxx"),
            NoSuchChild, WithMessage("Cannot access child! No such child in node: xxx"));
      }
      THEN("removing a child returns nullptr")
      {
        REQUIRE(node.removeChild("xxx") == nullptr);
      }
    }

    WHEN("a child added")
    {
      node.addChild("name", Entity::create<Leaf<int>>(12));
      THEN("size increased")
      {
        REQUIRE(node.size() == 1);
      }

      THEN("child can be accessed")
      {
        REQUIRE(types::LeafType<int>().get(node.getChild("name")).getValue() == 12);
      }

      THEN("the entry can be accessed")
      {
        REQUIRE(types::LeafType<int>().get(node.getEntry("name").second).getValue() == 12);
      }
    }
  }
}

TEST_CASE("remove child from node")
{
    Node node;
    node.addChild("name", Entity::create<Leaf<int>>(12));
    node.addChild("age", Entity::create<Leaf<int>>(15));
    REQUIRE(node.size() == 2);

    const auto removed = node.removeChild("name");
    REQUIRE(types::LeafType<int>().get(*removed).getValue() == 12);
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
        sum += types::LeafType<int>().get(child.second).getValue();
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
        sum += types::LeafType<int>().get(child.second).getValue();
    }

    REQUIRE(sum == 6);
}
