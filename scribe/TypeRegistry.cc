#include <scribe/TypeRegistry.h>
#include <scribe/exception.h>
#include <scribe/makeString.h>
#include <scribe/EntityFormatter.h>

using namespace scribe;

namespace
{
  class SimpleTypeNotion : public RegisterableTypeNotion
  {
    public:
      explicit SimpleTypeNotion(std::unique_ptr<TypeNotion>&& notion)
        : wrapped(std::move(notion))
      {}

      void validate(const Entity& entity, const ValidationContext&) const override
      { wrapped->validate(entity); }

    private:
      std::unique_ptr<TypeNotion> wrapped{nullptr};
  };
}

void TypeRegistry::registerType(const TypeName& name, std::unique_ptr<TypeNotion>&& type)
{
  if (types.find(name) != types.end())
    throw ScribeException(makeString() << "'" << name << "' is already registered!");

  types.emplace(name, std::make_unique<SimpleTypeNotion>(std::move(type)));
}

void TypeRegistry::registerType(const TypeName& name, std::unique_ptr<RegisterableTypeNotion>&& type)
{
  if (types.find(name) != types.end())
    throw ScribeException(makeString() << "'" << name << "' is already registered!");

  types.emplace(name, std::move(type));
}

const RegisterableTypeNotion& TypeRegistry::getType(const TypeName& name) const
{
  auto found = types.find(name);
  if (found == types.end())
    throw ScribeException(makeString() << "Type '" << name << "' is not registered!");

  return *(found->second);
}

void TypeRegistry::validate(const Entity& entity) const
{
  for (const auto& type : types)
  {
    try {
      type.second->validate(entity, ValidationContext{*this});
      return;
    }
    catch (const ScribeException& ex)
    {
      // it is just suppressed, it would be nicer if validate had a bool-returned version
    }
  }
  throw ScribeException(makeString() << "Validation failed for: " << printEntity(entity));
}
