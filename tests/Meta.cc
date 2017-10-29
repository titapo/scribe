#define CATCH_CONFIG_MAIN
#include <tests/catch.hpp>
#include <scribe/Meta.h>
#include <scribe/Leaf.h>

using namespace scribe;

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

TEST_CASE("type definition -- from node")
{
  meta::TypeDefinition def("Person");
  def.addField({"name", "string"});
  def.addField({"age", "unsigned"});

  Node node;
  def.addToNode(node);

  const auto& result = meta::TypeDefinition::fromNode(node);
  REQUIRE(result.getName() == "Person");
  REQUIRE(result.getFields().find("name") != result.getFields().end());
  REQUIRE(result.getFields().find("name")->second.type == "string");
  REQUIRE(result.getFields().find("age") != result.getFields().end());
  REQUIRE(result.getFields().find("age")->second.type == "unsigned");
}
