#define CATCH_CONFIG_MAIN
#include <tests/catch.hpp>
#include <scribe/Meta.h>
#include <scribe/Leaf.h>

using namespace scribe;

meta::TypeDefinition getPersonDefinition()
{
  meta::TypeDefinition def("Person");
  def.addField({"name", "string"});
  def.addField({"age", "unsigned"});
  return def;
}

TEST_CASE("type definition -- add to node")
{
  meta::TypeDefinition def("Person");
  Node node;

  SECTION("empty definition")
  {
    def.addToNode(node);
    REQUIRE(node.hasChild(meta::metaSpecifier));
    const auto& meta = dynamic_cast<const Node &>(node.getChild(meta::metaSpecifier));
    REQUIRE(meta.hasChild(meta::specifierKey));
    REQUIRE(dynamic_cast<const Leaf<std::string>&>(meta.getChild(meta::specifierKey)).getValue() == "type_definition");
    REQUIRE(meta.hasChild("name"));
    REQUIRE(dynamic_cast<const Leaf<std::string>&>(meta.getChild("name")).getValue() == "Person");
    REQUIRE(!node.hasChild("fields"));
  }

  SECTION("fields")
  {
    def.addField({"name", "string"});
    def.addField({"age", "unsigned"});

    def.addToNode(node);

    REQUIRE(node.hasChild("fields"));
    const auto& fields = dynamic_cast<const Node&>(node.getChild("fields"));
    REQUIRE(fields.size() == 2);
    REQUIRE(fields.hasChild("name"));
    REQUIRE(fields.hasChild("age"));
  }

  SECTION("fields must have different names")
  {
    def.addField({"name", "string"});
    REQUIRE_THROWS_AS(def.addField({"name", "unsigned"}), ScribeException);
    REQUIRE_THROWS_WITH(def.addField({"name", "unsigned"}), "Field already exists: 'name'!");
  }
}

#include <scribe/TypeNotion.h>

TEST_CASE("type definition -- from node")
{
  SECTION("from empty node")
  {
    Node node;
    REQUIRE_THROWS_AS(meta::TypeDefinition::fromNode(node), ScribeException); // TODO Some meta exception
    REQUIRE_THROWS_WITH(meta::TypeDefinition::fromNode(node), "No such child in node: ^^meta^^");
  }

  SECTION("to node and back -- without fields")
  {
    meta::TypeDefinition def("Person");

    Node node;
    def.addToNode(node);

    const auto& result = meta::TypeDefinition::fromNode(node);
    REQUIRE(result.getName() == "Person");
  }

  SECTION("to node and back -- with fields")
  {
    const auto& def = getPersonDefinition();

    Node node;
    def.addToNode(node);

    const auto& result = meta::TypeDefinition::fromNode(node);
    REQUIRE(result.getName() == "Person");
    REQUIRE(result.getFields().find("name") != result.getFields().end());
    REQUIRE(result.getFields().find("name")->second.type == "string");
    REQUIRE(result.getFields().find("age") != result.getFields().end());
    REQUIRE(result.getFields().find("age")->second.type == "unsigned");
  }

  SECTION("to node and back -- with fields, but removing specifier key from node")
  {
    const auto& def = getPersonDefinition();

    Node node;
    def.addToNode(node);

    types::NodeType().get(node.getChild(meta::metaSpecifier)).removeChild(meta::specifierKey);

    REQUIRE_THROWS_AS(meta::TypeDefinition::fromNode(node), ScribeException);
    REQUIRE_THROWS_WITH(meta::TypeDefinition::fromNode(node), "Missing meta specifier!");
  }

  SECTION("to node and back -- with fields, but altering specifier key")
  {
    const auto& def = getPersonDefinition();

    Node node;
    def.addToNode(node);

    auto& specifier = types::NodeType().get(node.getChild(meta::metaSpecifier)).getChild(meta::specifierKey);
    types::LeafType<std::string>().get(specifier).setValue("other thing");

    REQUIRE_THROWS_AS(meta::TypeDefinition::fromNode(node), ScribeException);
    REQUIRE_THROWS_WITH(meta::TypeDefinition::fromNode(node),
        "Invalid meta specifier: 'other thing' (expected: 'type_definition')!");
  }
}

#include <scribe/EntityFormatter.h>

TEST_CASE("type definition -- explicit exported content")
{
  Node node;
  getPersonDefinition().addToNode(node);
  std::stringstream output;
  EntityFormatter formatter(output, {});
  node.processBy(formatter);
  REQUIRE(output.str() == "{\n  fields: {\n    name: string\n    age: unsigned\n  }\n  ^^meta^^: {\n    name: Person\n    ?: type_definition\n  }\n}");
}

TEST_CASE("type reference -- add to node")
{

  meta::TypeReference ref("Person");
  Node node;

  ref.addToNode(node);
  REQUIRE(node.hasChild(meta::metaSpecifier));
  const auto& meta = types::NodeType().get(node.getChild(meta::metaSpecifier));
  REQUIRE(meta.hasChild(meta::specifierKey));
  REQUIRE(types::LeafType<std::string>().get(meta.getChild(meta::specifierKey)).getValue() == "type_ref");
  REQUIRE(meta.hasChild("type"));
  REQUIRE(types::LeafType<std::string>().get(meta.getChild("type")).getValue() == "Person");
}

TEST_CASE("type reference -- from node")
{

  SECTION("from empty node")
  {
    Node node;
    REQUIRE_THROWS_AS(meta::TypeReference::fromNode(node), ScribeException); // TODO Some meta exception
    REQUIRE_THROWS_WITH(meta::TypeReference::fromNode(node), "No such child in node: ^^meta^^");
  }

  SECTION("to node and back")
  {
    meta::TypeReference ref("Person");

    Node node;
    ref.addToNode(node);

    const auto& result = meta::TypeReference::fromNode(node);
    REQUIRE(result.getTypename() == "Person");
  }

  SECTION("to node and back -- but removing the specifer")
  {
    meta::TypeReference ref("Person");

    Node node;
    ref.addToNode(node);
    types::NodeType().get(node.getChild(meta::metaSpecifier)).removeChild(meta::specifierKey);

    REQUIRE_THROWS_AS(meta::TypeReference::fromNode(node), ScribeException);
    REQUIRE_THROWS_WITH(meta::TypeReference::fromNode(node), "Missing meta specifier!");
  }

  SECTION("to node and back -- but altering the specifer")
  {
    meta::TypeReference ref("Person");

    Node node;
    ref.addToNode(node);
    auto& specifier = types::NodeType().get(node.getChild(meta::metaSpecifier)).getChild(meta::specifierKey);
    types::LeafType<std::string>().get(specifier).setValue("other thing");

    REQUIRE_THROWS_AS(meta::TypeReference::fromNode(node), ScribeException);
    REQUIRE_THROWS_WITH(meta::TypeReference::fromNode(node), "Invalid meta specifier: 'other thing' (expected: 'type_ref')!");
  }

}
