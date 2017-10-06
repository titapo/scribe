#include <scribe/TypeRegistry.h>
#include <scribe/exception.h>

using namespace scribe;

void TypeRegistry::registerType(const TypeName& name, std::unique_ptr<TypeNotion>&& type)
{
  types.emplace(name, std::move(type));
}

const TypeNotion& TypeRegistry::getType(const TypeName& name) const
{
  auto found = types.find(name);
  if (found == types.end())
    throw ScribeException("Type is not registered!");

  return *(found->second);
}

