#define CATCH_CONFIG_MAIN
#include <tests/catch.hpp>
#include <scribe/Leaf.h>
#include <scribe/Node.h>
#include <scribe/Array.h>
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
    formatter.display(node);
    REQUIRE(str.str() == "{\n}");
}

TEST_CASE("display non-empty node")
{
    std::ostringstream str;
    EntityFormatter formatter(str);
    Node node;
    node.addChild("integer", Entity::create<Leaf<int>>(100));
    formatter.display(node);
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
    formatter.display(node);
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
    formatter.display(node);
    REQUIRE(str.str() == "{\n  child: {\n    double: 12.34\n    string: apple\n  }\n}");
}

TEST_CASE("display empty array")
{
    std::ostringstream str;
    EntityFormatter formatter(str);
    Array array;
    formatter.display(array);
    REQUIRE(str.str() == "[\n]");
}

TEST_CASE("display non-empty array")
{
    std::ostringstream str;
    EntityFormatter formatter(str);
    Array array;
    array.append(Entity::create<Leaf<unsigned>>(1234));
    formatter.display(array);
    REQUIRE(str.str() == "[\n  1234\n]");
}

TEST_CASE("display multi-element array")
{
    std::ostringstream str;
    EntityFormatter formatter(str);
    Array array;
    array.append(Entity::create<Leaf<unsigned>>(1234));
    array.append(Entity::create<Leaf<std::string>>("just a string"));
    array.append(Entity::create<Leaf<double>>(232.4));
    formatter.display(array);
    REQUIRE(str.str() == "[\n  1234\n  just a string\n  232.4\n]");
}

TEST_CASE("display nested array")
{
    std::ostringstream str;
    EntityFormatter formatter(str);
    Array array;
    array.append(Entity::create<Leaf<unsigned>>(1234));
    array.append(Entity::create<Array>());
    dynamic_cast<Array&>(array.getChild(1)).append(Entity::create<Leaf<double>>(232.4));
    formatter.display(array);
    REQUIRE(str.str() == "[\n  1234\n  [\n    232.4\n  ]\n]");
}
