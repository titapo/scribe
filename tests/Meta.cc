#include <tests/common.h>
#include <scribe/Array.h>
#include <scribe/Meta.h>
#include <scribe/Leaf.h>
#include <scribe/TypeNotion.h>

using namespace scribe;

meta::TypeDefinition getPersonDefinition()
{
  meta::TypeDefinition def("Person");
  def.addField({"name", TypeName("string")});
  def.addField({"age", TypeName("unsigned")});
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
    REQUIRE(!meta.hasChild("generics"));
  }

  SECTION("fields")
  {
    def.addField({"name", TypeName("string")});
    def.addField({"age", TypeName("unsigned")});

    def.addToNode(node);

    REQUIRE(node.hasChild("fields"));
    const auto& fields = dynamic_cast<const Node&>(node.getChild("fields"));
    REQUIRE(fields.size() == 2);
    REQUIRE(fields.hasChild("name"));
    REQUIRE(fields.hasChild("age"));
  }

  SECTION("fields must have different names")
  {
    def.addField({"name", TypeName("string")});
    REQUIRE_THROWS_AS(def.addField({"name", TypeName("unsigned")}), meta::MetaException);
    REQUIRE_THROWS_WITH(def.addField({"name", TypeName("unsigned")}), "Field already exists: 'name'!");
  }

  SECTION("can have generic (type) parameters")
  {
    def.addGeneric("T");
    def.addGeneric("U");
    def.addToNode(node);
    const auto& meta = types::NodeType().get(node.getChild(meta::metaSpecifier));
    const auto& generics = types::ArrayType().get(meta.getChild("generics"));
    REQUIRE(types::LeafType<std::string>().get(generics.getChild(0)).getValue() == "T");
    REQUIRE(types::LeafType<std::string>().get(generics.getChild(1)).getValue() == "U");
  }

  SECTION("generic parameters must have different names")
  {
    def.addGeneric("T");
    REQUIRE_THROWS_AS(def.addGeneric("T"), meta::MetaException);
    REQUIRE_THROWS_WITH(def.addGeneric("T"), "'T' is repeated!");
  }

  SECTION("specify generics")
  {
    def.addField({"something", TypeName("T")});
    def.addGeneric("T");
    const auto& specified = def.specialize({TypeName("integer")});
    REQUIRE(specified.getName().get() == "Person<integer>");
    REQUIRE(specified.getGenerics().size() == 0);
    REQUIRE(specified.getFields().size() == def.getFields().size());
    REQUIRE(specified.getFields().find("something")->second.type.get() == "integer");
  }

  SECTION("specify generics for a non-generic definition")
  {
    REQUIRE_THROWS_AS(def.specialize({}), meta::MetaException);
    REQUIRE_THROWS_WITH(def.specialize({}), "Cannot specialize a non-generic definition");
  }

  SECTION("specify more generics than expected")
  {
    def.addGeneric("T");
    REQUIRE_THROWS_AS(def.specialize({TypeName("T"), TypeName("U")}), meta::MetaException);
    REQUIRE_THROWS_WITH(def.specialize({TypeName("T"), TypeName("U")}), "'Person' expected 1 generic(s), but 2 provided!");
  }

  SECTION("specify less generics than expected")
  {
    def.addGeneric("T");
    def.addGeneric("U");
    REQUIRE_THROWS_AS(def.specialize({TypeName("T")}), meta::MetaException);
    REQUIRE_THROWS_WITH(def.specialize({TypeName("T")}), "'Person' expected 2 generic(s), but 1 provided!");
  }
  // SECTION("specify less generics")
}

#include <scribe/TypeNotion.h>

TEST_CASE("type definition -- from node")
{
  SECTION("from empty node")
  {
    Node node;
    REQUIRE_THROWS_AS(meta::TypeDefinition::fromNode(node), meta::MetaException);
    REQUIRE_THROWS_WITH(meta::TypeDefinition::fromNode(node), "Missing meta key!");
  }

  SECTION("to node and back -- without fields")
  {
    meta::TypeDefinition def("Person");

    Node node;
    def.addToNode(node);

    const auto& result = meta::TypeDefinition::fromNode(node);
    REQUIRE(result.getName().get() == "Person");
  }

  SECTION("to node and back -- with fields")
  {
    const auto& def = getPersonDefinition();

    Node node;
    def.addToNode(node);

    const auto& result = meta::TypeDefinition::fromNode(node);
    REQUIRE(result.getName().get() == "Person");
    REQUIRE(result.getFields().find("name") != result.getFields().end());
    REQUIRE(result.getFields().find("name")->second.type.get() == "string");
    REQUIRE(result.getFields().find("age") != result.getFields().end());
    REQUIRE(result.getFields().find("age")->second.type.get() == "unsigned");
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

  SECTION("to node and back -- generics")
  {
    auto def = getPersonDefinition();
    def.addGeneric("T");
    def.addGeneric("U");

    Node node;
    def.addToNode(node);

    const auto result = meta::TypeDefinition::fromNode(node);
    const auto& generics = result.getGenerics();
    REQUIRE(generics.size() == 2);
    REQUIRE(generics.at(0) == "T");
    REQUIRE(generics.at(1) == "U");
  }
}

#include <scribe/EntityFormatter.h>

TEST_CASE("type definition -- explicit exported content")
{
  Node node;
  getPersonDefinition().addToNode(node);
  std::stringstream output;
  EntityFormatter formatter(output);
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
    REQUIRE_THROWS_AS(meta::TypeReference::fromNode(node), meta::MetaException);
    REQUIRE_THROWS_WITH(meta::TypeReference::fromNode(node), "Missing meta key!");
  }

  SECTION("to node and back")
  {
    meta::TypeReference ref("Person");

    Node node;
    ref.addToNode(node);

    const auto& result = meta::TypeReference::fromNode(node);
    REQUIRE(result.getTypename().get() == "Person");
  }

  SECTION("to node and back -- but removing the specifer")
  {
    meta::TypeReference ref("Person");

    Node node;
    ref.addToNode(node);
    types::NodeType().get(node.getChild(meta::metaSpecifier)).removeChild(meta::specifierKey);

    REQUIRE_THROWS_AS(meta::TypeReference::fromNode(node), meta::MetaException);
    REQUIRE_THROWS_WITH(meta::TypeReference::fromNode(node), "Missing meta specifier!");
  }

  SECTION("to node and back -- but altering the specifer")
  {
    meta::TypeReference ref("Person");

    Node node;
    ref.addToNode(node);
    auto& specifier = types::NodeType().get(node.getChild(meta::metaSpecifier)).getChild(meta::specifierKey);
    types::LeafType<std::string>().get(specifier).setValue("other thing");

    REQUIRE_THROWS_AS(meta::TypeReference::fromNode(node), meta::MetaException);
    REQUIRE_THROWS_WITH(meta::TypeReference::fromNode(node), "Invalid meta specifier: 'other thing' (expected: 'type_ref')!");
  }

}
