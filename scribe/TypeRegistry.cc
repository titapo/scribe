#include <scribe/TypeRegistry.h>
#include <scribe/exception.h>
#include <scribe/makeString.h>
#include <scribe/EntityFormatter.h>

using namespace scribe;

void TypeRegistry::registerType(const TypeName& name, std::unique_ptr<TypeNotion>&& type)
{
  if (types.find(name) != types.end())
    throw ScribeException(makeString() << "'" << name << "' is already registered!");

  types.emplace(name, std::move(type));
}

const TypeNotion& TypeRegistry::getType(const TypeName& name) const
{
  auto found = types.find(name);
  if (found == types.end())
    throw ScribeException("Type is not registered!");

  return *(found->second);
}

void TypeRegistry::validate(const Entity& entity) const
{
  for (const auto& type : types)
  {
    try {
      type.second->validate(entity);
      return;
    }
    catch (const ScribeException& ex)
    {
      // it is just suppressed, it would be nicer if validate had a bool-returned version
    }
  }
  throw ScribeException(makeString() << "Validation failed for: " << printEntity(entity));
}
