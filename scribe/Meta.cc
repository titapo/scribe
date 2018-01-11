#include "Meta.h"
#include "Leaf.h"
#include <scribe/Array.h>
#include <scribe/TypeNotion.h>
#include <scribe/makeString.h>
#include <algorithm>

using namespace scribe;
using namespace scribe::meta;

const Node& getMeta(const Node& node)
{
  if (!node.hasChild(metaSpecifier))
    throw MetaException(makeString() << "Missing meta key!");

  const auto& meta = node.getChild(std::string(metaSpecifier));
  return types::NodeType().get(meta);
}

Node& getMeta(Node& node)
{
  if (!node.hasChild(metaSpecifier))
    throw MetaException(makeString() << "Missing meta key!");

  auto& meta = node.getChild(std::string(metaSpecifier));
  return types::NodeType().get(meta);
}

void assertMetaSpecifier(const Node& meta, const std::string& expectedSpecifier)
{
  if (!meta.hasChild(specifierKey))
    throw MetaException(makeString() << "Missing meta specifier!");

  const auto& specifier = types::LeafType<std::string>().get(meta.getChild(specifierKey)).getValue();
  if (specifier != expectedSpecifier)
    throw MetaException(makeString() << "Invalid meta specifier: '" << specifier
        << "' (expected: '" << expectedSpecifier<< "')!");
}

std::unique_ptr<Node> createMeta(const std::string& specifier)
{
  auto meta = std::make_unique<Node>();
  meta->addChild(specifierKey, Entity::create<Leaf<std::string>>(specifier));
  return meta;
}

// TODO split smaller parts // hide in cc
void BasicTypeDefinition::addToNode(Node& node) const
{
  auto meta = std::make_unique<Node>();
  meta->addChild(specifierKey, Entity::create<Leaf<std::string>>("type_definition"));
  meta->addChild("name", Entity::create<Leaf<std::string>>(getName().get()));
  node.addChild(std::string(metaSpecifier), std::move(meta));

  if (!getFields().empty())
  {
    auto fieldsNode = std::make_unique<Node>();
    for (const auto& field : getFields() | pick_second())
    {
      fieldsNode->addChild(field.name, Entity::create<Leaf<std::string>>(field.type.get()));
    }
      
    node.addChild("fields", std::move(fieldsNode));
  }
}

void TypeDefinition::addToNode(Node& node) const
{
  BasicTypeDefinition::addToNode(node);
}

void BasicTypeDefinition::addField(const BasicTypeDefinition::Field& field)
{
  if (fields.find(field.name) != fields.end())
    throw MetaException(makeString() << "Field already exists: '" << field.name << "'!");

  fields.emplace(field.name, field);
}

void BasicTypeDefinition::addField(BasicTypeDefinition::Field&& field)
{
  if (fields.find(field.name) != fields.end())
    throw MetaException(makeString() << "Field already exists: '" << field.name << "'!");

  fields.emplace(field.name, std::move(field));
}

namespace
{
  void assertGenericList(const GenericTypeDefinition::Generics& generics)
  {
    if (generics.empty())
      throw MetaException("GenericTypeDefinition must have at least one generic parameter!");

    const auto duplicated = find_duplicated(generics);
    if (duplicated != generics.end())
        throw MetaException(makeString() << "'" << *duplicated << "' is repeated!");
  }
}

GenericTypeDefinition::GenericTypeDefinition(const TypeName& typeName, Generics&& genericList)
  : BasicTypeDefinition(std::move(typeName))
  , generics(genericList)
{
  assertGenericList(generics);
} 

GenericTypeDefinition::GenericTypeDefinition(TypeName&& typeName, Generics&& genericList)
  : BasicTypeDefinition(std::move(typeName))
  , generics(genericList)
{
  assertGenericList(generics);
} 

void GenericTypeDefinition::addToNode(Node& node) const
{
  BasicTypeDefinition::addToNode(node);

  auto genericsList = std::make_unique<Array>();
  for (const auto& generic : getGenerics())
  {
    genericsList->append(Entity::create<Leaf<std::string>>(generic));
  }
  getMeta(node).addChild("generics", std::move(genericsList));
}

const GenericTypeDefinition::Generics& GenericTypeDefinition::getGenerics() const
{ return generics; }

namespace
{
  GenericTypeDefinition::Generics loadGenericsFromMeta(const Node& meta)
  {
    if (!meta.hasChild("generics"))
      throw MetaException("No generics specified!");

    const auto& generics = types::ArrayType().get(meta.getChild("generics"));
    GenericTypeDefinition::Generics result;

    // TODO transform?
    std::transform(generics.begin(), generics.end(), std::back_inserter(result),
        [](const Entity& name) { return types::LeafType<std::string>().get(name).getValue(); });

    return result;
  }

  void loadFieldsFromNode(BasicTypeDefinition& def, const Node& node)
  {
    if (!node.hasChild("fields"))
      return;

    const auto& fieldsNode = types::NodeType().get(node.getChild("fields"));
    for (const auto& fieldEntry : fieldsNode)
      def.addField(BasicTypeDefinition::Field(fieldEntry.first, TypeName(types::LeafType<std::string>().get(fieldEntry.second).getValue())));
  }
}

TypeDefinition TypeDefinition::fromNode(const Node& node)
{
  const auto& meta = getMeta(node);
  assertMetaSpecifier(meta, "type_definition");
  TypeDefinition def(types::LeafType<std::string>().get(meta.getChild("name")).getValue());

  loadFieldsFromNode(def, node);
  return def;
}

GenericTypeDefinition GenericTypeDefinition::fromNode(const Node& node)
{
  const auto& meta = getMeta(node);
  assertMetaSpecifier(meta, "type_definition");
  GenericTypeDefinition def(
      TypeName(types::LeafType<std::string>().get(meta.getChild("name")).getValue()),
      loadGenericsFromMeta(meta));

  loadFieldsFromNode(def, node);
  return def;
}

namespace
{
  BasicTypeDefinition::Field specializeField(const GenericTypeDefinition& original, const BasicTypeDefinition::Field& field, const std::vector<TypeName> specializations)
  {
    const auto referred = find_in(original.getGenerics(), field.type.get());
    if (referred == original.getGenerics().end())
      return {field};

    const auto& index = std::distance(original.getGenerics().begin(), referred);
    return {field.name, TypeName(specializations.at(index))};
  }
  void specializeFields(const GenericTypeDefinition& original, const std::vector<TypeName> specializations, BasicTypeDefinition& specialized)
  {
    for (const auto& field : original.getFields() | pick_second())
      specialized.addField(specializeField(original, field, specializations));
  }

  TypeName specializeName(const TypeName& type, const std::vector<TypeName>& specializations)
  {
    return TypeName(makeString() << type.get() << "<"<< join(specializations | toUnderlying(), ", ") << ">");
  }

  TypeDefinition specializeTypeDefinition(const GenericTypeDefinition& original, const std::vector<TypeName>& specializations)
  {
    TypeDefinition def{specializeName(original.getName(), specializations)};
    specializeFields(original, specializations, def);
    return def;
  }
}

TypeDefinition GenericTypeDefinition::specialize(const std::vector<TypeName>& specializations) const
{
  if (generics.size() == 0) // This is not supposed to happen // THIS IS ALREADY EXCLUDED
    throw MetaException("Cannot specialize a non-generic definition");
  
  if (specializations.size() != generics.size())
    throw MetaException(makeString() << "'" << getName().get() << "' expected " << generics.size()
        << " generic(s), but " << specializations.size() << " provided!");

  return specializeTypeDefinition(*this, specializations);
}

void TypeReference::addToNode(Node& node) const
{
  auto meta = createMeta("type_ref");
  meta->addChild("type", Entity::create<Leaf<std::string>>(type.get()));
  node.addChild(std::string(metaSpecifier), std::move(meta));
}

TypeReference TypeReference::fromNode(const Node& node)
{
  const auto& meta = getMeta(node);
  assertMetaSpecifier(meta, "type_ref");
  return TypeReference(types::LeafType<std::string>().get(meta.getChild("type")).getValue());
}

TypeName TypeReference::getTypename() const
{
  return type;
}
