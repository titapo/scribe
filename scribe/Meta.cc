#include "Meta.h"
#include "Leaf.h"
#include <scribe/makeString.h>
#include <scribe/TypeNotion.h>

using namespace scribe;
using namespace scribe::meta;

const Node& getMeta(const Node& node)
{
  const auto& meta = node.getChild(std::string(metaSpecifier));
  return types::NodeType().get(meta);
}

void assertMetaSpecifier(const Node& meta, const std::string& expectedSpecifier)
{
  if (!meta.hasChild(specifierKey))
    throw ScribeException(makeString() << "Missing meta specifier!");

  const auto& specifier = types::LeafType<std::string>().get(meta.getChild(specifierKey)).getValue();
  if (specifier != expectedSpecifier)
    throw ScribeException(makeString() << "Invalid meta specifier: '" << specifier
        << "' (expected: '" << expectedSpecifier<< "')!");
}

std::unique_ptr<Node> createMeta(const std::string& specifier)
{
  auto meta = std::make_unique<Node>();
  meta->addChild(specifierKey, Entity::create<Leaf<std::string>>(specifier));
  return meta;
}

void TypeDefinition::addToNode(Node& node) const
{
  auto meta = std::make_unique<Node>();
  meta->addChild(specifierKey, Entity::create<Leaf<std::string>>("type_definition"));
  meta->addChild("name", Entity::create<Leaf<std::string>>(name));
  node.addChild(std::string(metaSpecifier), std::move(meta));
  if (!fields.empty())
  {
    auto fieldsNode = std::make_unique<Node>();
    for (const auto& field : fields)
    {
      fieldsNode->addChild(field.first, Entity::create<Leaf<std::string>>(field.second.type));
    }
      
    node.addChild("fields", std::move(fieldsNode));
  }
}

void TypeDefinition::addField(TypeDefinition::Field&& field)
{
  if (fields.find(field.name) != fields.end())
    throw ScribeException(makeString() << "Field already exists: '" << field.name << "'!");

  fields.emplace(field.name, std::move(field));
}

TypeDefinition TypeDefinition::fromNode(const Node& node)
{
  const auto& meta = getMeta(node);
  assertMetaSpecifier(meta, "type_definition");
  TypeDefinition def(types::LeafType<std::string>().get(meta.getChild("name")).getValue());
  if (!node.hasChild("fields"))
    return def;

  const auto& fieldsNode = types::NodeType().get(node.getChild("fields"));
  for (const auto& fieldEntry : fieldsNode)
  {
    def.addField({fieldEntry.first, types::LeafType<std::string>().get(fieldEntry.second).getValue()});
  }
  return def;
}

void TypeReference::addToNode(Node& node) const
{
  auto meta = createMeta("type_ref");
  meta->addChild("type", Entity::create<Leaf<std::string>>(type));
  node.addChild(std::string(metaSpecifier), std::move(meta));
}

TypeReference TypeReference::fromNode(const Node& node)
{
  const auto& meta = getMeta(node);
  assertMetaSpecifier(meta, "type_ref");
  return TypeReference(types::LeafType<std::string>().get(meta.getChild("type")).getValue());
}

TypeReference::TypeName TypeReference::getTypename() const
{
  return type;
}
