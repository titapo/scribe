#define CATCH_CONFIG_MAIN
#include <tests/catch.hpp>
#include <scribe/Node.h>
//#include <scribe/Array.h>
//#include <scribe/Leaf.h>
#include <scribe/ComplexTypeNotion.h>
#include <scribe/exception.h>

using namespace scribe;

TEST_CASE("validate node")
{
    meta::TypeDefinition def("node");
    types::ComplexTypeNotion notion{def};

    REQUIRE_THROWS_AS(notion.validate(Node()), meta::MetaException);
    REQUIRE_THROWS_WITH(notion.validate(Node()), "Missing meta key!");
}

TEST_CASE("validate node with other reference")
{
    meta::TypeDefinition def("node");
    types::ComplexTypeNotion notion{def};
    Node node;

    meta::TypeReference("other_ref").addToNode(node);

    REQUIRE_THROWS_AS(notion.validate(node), ScribeException);
    REQUIRE_THROWS_WITH(notion.validate(node), "Validation failed! 'node' passed when 'other_ref' was expected!");
}

TEST_CASE("validate node with same reference")
{
    meta::TypeDefinition def("node");
    types::ComplexTypeNotion notion{def};
    Node node;

    meta::TypeReference("node").addToNode(node);

    REQUIRE_NOTHROW(notion.validate(node));
}

TEST_CASE("validate node without required fields")
{
    meta::TypeDefinition def("Person");
    def.addField({"name", "string"});
    types::ComplexTypeNotion notion{def};

    Node node;
    meta::TypeReference("Person").addToNode(node);

    REQUIRE_THROWS_AS(notion.validate(node), ScribeException);
    REQUIRE_THROWS_WITH(notion.validate(node), "Validation failed! 'Person' must have field: 'name' (string)!");
}
