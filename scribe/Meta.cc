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
  // TODO check specifier key
  TypeDefinition def(types::LeafType<std::string>().get(meta.getChild("name")).getValue());
  const auto& fieldsNode = types::NodeType().get(node.getChild("fields"));
  for (const auto& fieldEntry : fieldsNode)
  {
    def.addField({fieldEntry.first, types::LeafType<std::string>().get(fieldEntry.second).getValue()});
  }
  return def;
}
