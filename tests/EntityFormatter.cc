#define CATCH_CONFIG_MAIN
#include <tests/catch.hpp>
#include <scribe/Leaf.h>
#include <scribe/Node.h>
#include <scribe/EntityFormatter.h>

using namespace scribe;

TEST_CASE("display leaf")
{
    std::ostringstream str;
    EntityFormatter formatter(str);
    Leaf<int> leaf(123);
    leaf.processBy(formatter);
    REQUIRE(str.str() == "123");
}

TEST_CASE("display leaf entry")
{
    std::ostringstream str;
    EntityFormatter formatter(str);
    Leaf<int> leaf(123);
    Node::WeakEntry entry("my leaf", leaf);
    formatter.display(entry);
    REQUIRE(str.str() == "my leaf: 123");
}

TEST_CASE("display empty node")
{
    std::ostringstream str;
    EntityFormatter formatter(str);
    Node node;
    node.processBy(formatter);
    REQUIRE(str.str() == "{\n}");
}

TEST_CASE("display non-empty node")
{
    std::ostringstream str;
    EntityFormatter formatter(str);
    Node node;
    node.addChild("integer", Entity::create<Leaf<int>>(100));
    node.processBy(formatter);
    REQUIRE(str.str() == "{\n  integer: 100\n}");
}

TEST_CASE("display multi-element node")
{
    std::ostringstream str;
    EntityFormatter formatter(str);
    Node node;
    node.addChild("integer", Entity::create<Leaf<int>>(100));
    node.addChild("string", Entity::create<Leaf<std::string>>("apple"));
    node.addChild("double", Entity::create<Leaf<double>>(12.34));
    node.processBy(formatter);
    REQUIRE(str.str() == "{\n  double: 12.34\n  integer: 100\n  string: apple\n}");
}

TEST_CASE("display nested node")
{
    std::ostringstream str;
    EntityFormatter formatter(str);
    Node node;
    node.addChild("child", Entity::create<Node>());
    auto& childRef = dynamic_cast<Node&>(node.getChild("child"));
    childRef.addChild("string", Entity::create<Leaf<std::string>>("apple"));
    childRef.addChild("double", Entity::create<Leaf<double>>(12.34));
    node.processBy(formatter);
    REQUIRE(str.str() == "{\n  child: {\n    double: 12.34\n    string: apple\n  }\n}");
}
