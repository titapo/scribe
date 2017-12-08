#include "Meta.h"
#include "Leaf.h"
#include <scribe/Array.h>
#include <scribe/TypeNotion.h>
#include <scribe/makeString.h>
#include <algorithm>
#include <numeric> // for accumulate

using namespace scribe;
using namespace scribe::meta;

const Node& getMeta(const Node& node)
{
  if (!node.hasChild(metaSpecifier))
    throw MetaException(makeString() << "Missing meta key!");

  const auto& meta = node.getChild(std::string(metaSpecifier));
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

void TypeDefinition::addToNode(Node& node) const
{
  auto meta = std::make_unique<Node>();
  meta->addChild(specifierKey, Entity::create<Leaf<std::string>>("type_definition"));
  meta->addChild("name", Entity::create<Leaf<std::string>>(name));

  if (!generics.empty())
  {
    auto genericsList = std::make_unique<Array>();
    for (const auto& generic : generics)
    {
      genericsList->append(Entity::create<Leaf<std::string>>(generic));
    }
    meta->addChild("generics", std::move(genericsList));
  }
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

void TypeDefinition::addField(const TypeDefinition::Field& field)
{
  if (fields.find(field.name) != fields.end())
    throw MetaException(makeString() << "Field already exists: '" << field.name << "'!");

  fields.emplace(field.name, field);
}

void TypeDefinition::addField(TypeDefinition::Field&& field)
{
  if (fields.find(field.name) != fields.end())
    throw MetaException(makeString() << "Field already exists: '" << field.name << "'!");

  fields.emplace(field.name, std::move(field));
}

void TypeDefinition::addGeneric(const GenericName& generic)
{
  if (std::find(generics.begin(), generics.end(), generic) != generics.end())
    throw MetaException(makeString() << "'" << generic << "' is repeated!");

  generics.push_back(generic);
}

namespace
{
  void loadGenericsFromMeta(TypeDefinition& def, const Node& meta)
  {
    if (!meta.hasChild("generics"))
      return;

    const auto& generics = types::ArrayType().get(meta.getChild("generics"));
    for (const auto& generic : generics)
      def.addGeneric(types::LeafType<std::string>().get(generic).getValue());
  }

  void loadFieldsFromNode(TypeDefinition& def, const Node& node)
  {
    if (!node.hasChild("fields"))
      return;

    const auto& fieldsNode = types::NodeType().get(node.getChild("fields"));
    for (const auto& fieldEntry : fieldsNode)
      def.addField({fieldEntry.first, types::LeafType<std::string>().get(fieldEntry.second).getValue()});
  }
}

TypeDefinition TypeDefinition::fromNode(const Node& node)
{
  const auto& meta = getMeta(node);
  assertMetaSpecifier(meta, "type_definition");
  TypeDefinition def(types::LeafType<std::string>().get(meta.getChild("name")).getValue());

  loadGenericsFromMeta(def, meta);
  loadFieldsFromNode(def, node);
  return def;
}

namespace
{
  // TODO move to makeString()?
  template <typename Range>
  std::string join(const Range& range, const std::string& joiner)
  {
    if (range.empty())
      return {};

    return std::accumulate(range.begin() + 1, range.end(),
        range.at(0), [&joiner](const auto& a, const auto& b) -> std::string
        { return makeString() << a << joiner << b; });
  }

  // TODO common
  template <typename Range>
  auto find_in(const Range& range, const typename Range::value_type& needle)
  {
    return std::find(range.begin(), range.end(), needle);
  }

  TypeDefinition::Field specializeField(const TypeDefinition& original, const TypeDefinition::Field& field, const std::vector<TypeName> specializations)
  {
    const auto referred = find_in(original.getGenerics(), field.type);
    if (referred == original.getGenerics().end())
      return {field};

    const auto& index = std::distance(original.getGenerics().begin(), referred);
    return {field.name, specializations.at(index)};
  }
  void specializeFields(const TypeDefinition& original, const std::vector<TypeName> specializations, TypeDefinition& specialized)
  {
    for (const auto& field : original.getFields())
      specialized.addField(specializeField(original, field.second, specializations));
  }

  std::string specializeName(const TypeName& type, const std::vector<TypeName>& specializations)
  {
    return makeString() << type << "<"<< join(specializations, ", ") << ">";
  }

  TypeDefinition specializeTypeDefinition(const TypeDefinition& original, const std::vector<TypeName>& specializations)
  {
    TypeDefinition def{specializeName(original.getName(), specializations)};
    specializeFields(original, specializations, def);
    return def;
  }
}

TypeDefinition TypeDefinition::specialize(const std::vector<TypeName>& specializations) const
{
  if (generics.size() == 0) // TODO isGeneric()
    throw MetaException("Cannot specialize a non-generic definition");
  
  if (specializations.size() != generics.size())
    throw MetaException(makeString() << "'" << getName() << "' expected " << generics.size()
        << " generic(s), but " << specializations.size() << " provided!");

  return specializeTypeDefinition(*this, specializations);

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
