#include <tests/common.h>
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
    const ValidationContext ctx{};

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
        def.addField({"name", TypeName("string")});
        types::ComplexTypeNotion notion{def, registry};

        Node node;
        meta::TypeReference("Person").addToNode(node);

        REQUIRE_THROWS_AS(notion.validate(node, ctx), ScribeException); // Validation failed
        REQUIRE_THROWS_WITH(notion.validate(node, ctx), "Validation failed! 'Person' must have field: 'name' (string)!");
    }

    SECTION("validate node with non-registered typed field")
    {
        meta::TypeDefinition def("Person");
        def.addField({"name", TypeName("mystring")});
        REQUIRE_THROWS_AS(types::ComplexTypeNotion(def, registry), ScribeException);
        REQUIRE_THROWS_WITH(types::ComplexTypeNotion(def, registry), "Type 'mystring' is not registered!");
    }

    SECTION("validate node with wrong typed field")
    {
        meta::TypeDefinition def("Person");
        def.addField({"name", TypeName("string")});
        types::ComplexTypeNotion notion{def, registry};

        Node node;
        meta::TypeReference("Person").addToNode(node);
        node.addChild("name", Entity::create<Leaf<int>>(1));

        REQUIRE_THROWS_AS(notion.validate(node, ctx), TypeValidationError);
        REQUIRE_THROWS_WITH(notion.validate(node, ctx),
            "Validation failed! In Person field 'name' is not a(n) 'string'! Not a proper leaf!");
    }

    SECTION("validate node with wrong typed field")
    {
        meta::TypeDefinition def("Person");
        def.addField({"name", TypeName("string")});
        types::ComplexTypeNotion notion{def, registry};

        Node node;
        meta::TypeReference("Person").addToNode(node);
        node.addChild("name", Entity::create<Leaf<std::string>>("Joe"));

        REQUIRE_NOTHROW(notion.validate(node, ctx));
    }

    SECTION("validate nested type")
    {
      meta::TypeDefinition addr("Address");
      addr.addField({"country", TypeName("string")});
      addr.addField({"street", TypeName("string")});
      registry.registerType("Address", std::make_unique<types::ComplexTypeNotion>(addr, registry));

      meta::TypeDefinition person("Person");
      person.addField({"name", TypeName("string")});
      person.addField({"address", TypeName("Address")});
      types::ComplexTypeNotion notion{person, registry};

      auto addrNode = std::make_unique<Node>();
      meta::TypeReference("Address").addToNode(*addrNode);
      addrNode->addChild("country", Entity::create<Leaf<std::string>>("XYZ"));
      addrNode->addChild("street", Entity::create<Leaf<std::string>>("One Street"));

      Node node;
      meta::TypeReference("Person").addToNode(node);
      node.addChild("name", Entity::create<Leaf<std::string>>("Joe"));
      node.addChild("address", std::move(addrNode));

      REQUIRE_NOTHROW(notion.validate(node, ctx));
    }

    SECTION("prohibit extra fields")
    {
        meta::TypeDefinition def("Person");
        def.addField({"name", TypeName("string")});
        types::ComplexTypeNotion notion{def, registry};

        Node node;
        meta::TypeReference("Person").addToNode(node);
        node.addChild("name", Entity::create<Leaf<std::string>>("Joe"));
        node.addChild("weight", Entity::create<Leaf<int>>(123));

        REQUIRE_THROWS_AS(notion.validate(node, ctx), TypeValidationError);
        REQUIRE_THROWS_WITH(notion.validate(node, ctx),
            "Validation failed! 'Person' should not have field named: 'weight'!");
    }

    SECTION("allow extra fields in case of Lazy validation")
    {
        meta::TypeDefinition def("Person");
        def.addField({"name", TypeName("string")});
        types::ComplexTypeNotion notion{def, registry};

        Node node;
        meta::TypeReference("Person").addToNode(node);
        node.addChild("name", Entity::create<Leaf<std::string>>("Joe"));
        node.addChild("weight", Entity::create<Leaf<int>>(123));

        REQUIRE_NOTHROW(notion.validate(node, {ValidationContext::Strictness::Lazy}));
    }
}
