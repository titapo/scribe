#define CATCH_CONFIG_MAIN
#include <tests/catch.hpp>
#include <scribe/Node.h>
//#include <scribe/Array.h>
//#include <scribe/Leaf.h>
#include <scribe/ComplexTypeNotion.h>
#include <scribe/TypeRegistry.h>
#include <scribe/exception.h>

using namespace scribe;

TEST_CASE("complex type tests")
{
    TypeRegistry registry;
    registry.registerType("string", std::make_unique<types::LeafType<std::string>>());
    ValidationContext ctx{registry};

    SECTION("validate node")
    {
        meta::TypeDefinition def("node");
        types::ComplexTypeNotion notion{def, registry};

        REQUIRE_THROWS_AS(notion.validate(Node(), ctx), meta::MetaException);
        REQUIRE_THROWS_WITH(notion.validate(Node(), ctx), "Missing meta key!");
    }

    SECTION("validate node with other reference")
    {
        meta::TypeDefinition def("node");
        types::ComplexTypeNotion notion{def, registry};
        Node node;

        meta::TypeReference("other_ref").addToNode(node);

        REQUIRE_THROWS_AS(notion.validate(node, ctx), ScribeException); // TODO ValidationError
        REQUIRE_THROWS_WITH(notion.validate(node, ctx), "Validation failed! 'node' passed when 'other_ref' was expected!");
    }

    SECTION("validate node with same reference")
    {
        meta::TypeDefinition def("node");
        types::ComplexTypeNotion notion{def, registry};
        Node node;

        meta::TypeReference("node").addToNode(node);

        REQUIRE_NOTHROW(notion.validate(node, ctx));
    }

    SECTION("validate node without required field")
    {
        meta::TypeDefinition def("Person");
        def.addField({"name", "string"});
        types::ComplexTypeNotion notion{def, registry};

        Node node;
        meta::TypeReference("Person").addToNode(node);

        REQUIRE_THROWS_AS(notion.validate(node, ctx), ScribeException); // Validation failed
        REQUIRE_THROWS_WITH(notion.validate(node, ctx), "Validation failed! 'Person' must have field: 'name' (string)!");
    }

    SECTION("validate node with non-registered typed field")
    {
        meta::TypeDefinition def("Person");
        def.addField({"name", "mystring"});
        REQUIRE_THROWS_AS(types::ComplexTypeNotion(def, registry), ScribeException);
        REQUIRE_THROWS_WITH(types::ComplexTypeNotion(def, registry), "Type 'mystring' is not registered!");
    }

    SECTION("validate node with wrong typed field")
    {
        meta::TypeDefinition def("Person");
        def.addField({"name", "string"});
        types::ComplexTypeNotion notion{def, registry};

        Node node;
        meta::TypeReference("Person").addToNode(node);
        node.addChild("name", Entity::create<Leaf<int>>(1));

        REQUIRE_THROWS_AS(notion.validate(node, ctx), TypeValidationError); // TODO ValidationError
        REQUIRE_THROWS_WITH(notion.validate(node, ctx),
            "Validation failed! In Person field 'name' is not a(n) 'string'! Not a proper leaf!");
    }

    SECTION("validate node with wrong typed field")
    {
        meta::TypeDefinition def("Person");
        def.addField({"name", "string"});
        types::ComplexTypeNotion notion{def, registry};

        Node node;
        meta::TypeReference("Person").addToNode(node);
        node.addChild("name", Entity::create<Leaf<std::string>>("Joe"));

        REQUIRE_NOTHROW(notion.validate(node, ctx));
    }
}
