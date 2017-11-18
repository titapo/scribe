#include <scribe/ComplexTypeNotion.h>
#include <scribe/makeString.h>

using namespace scribe;

types::ComplexTypeNotion::ComplexTypeNotion(const TypeDefinition& def)
  : definition(def)
{}

types::ComplexTypeNotion::ComplexTypeNotion(TypeDefinition&& def)
  : definition(std::move(def))
{}

void types::ComplexTypeNotion::validate(const Entity& entity) const
{
  nodeType.validate(entity);
  try
  {
    const auto& node = nodeType.get(entity);

    const auto& ref = meta::TypeReference::fromNode(node);
    if (ref.getTypename() != definition.getName())
      throw ScribeException(makeString() << "Validation failed! '"
          << definition.getName() << "' passed when '" << ref.getTypename() << "' was expected!");

    for (const auto& field : definition.getFields())
    {
      if (!node.hasChild(field.first))
        throw ScribeException(makeString() << "Validation failed! '" << definition.getName() << "' must have field: '"
            << field.first << "' (" << field.second.type << ")!");
    }
      
  }
  catch (const meta::MetaException& ex)
  {
    throw;
  }
}

std::unique_ptr<Entity> types::ComplexTypeNotion::instantiate()
{
  return nodeType.instantiate();
}

