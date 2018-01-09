#include <scribe/TypeRegistry.h>
#include <scribe/exception.h>
#include <scribe/makeString.h>
#include <scribe/EntityFormatter.h>
#include <scribe/ComplexTypeNotion.h> // TypeRegistry should not know this

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

  struct toTypeEntry
  {
    auto operator()(const TypeRegistry::Types::const_iterator& iter)
    {
      return TypeRegistry::TypeEntry(TypeName(iter->first), *(iter->second));
    }
  };
}

void TypeRegistry::registerType(const std::string& name, std::unique_ptr<TypeNotion>&& type)
{
  if (types.find(name) != types.end())
    throw ScribeException(makeString() << "'" << name << "' is already registered!");

  types.emplace(name, std::make_unique<SimpleTypeNotion>(std::move(type)));
}

void TypeRegistry::registerType(const std::string& name, std::unique_ptr<RegisterableTypeNotion>&& type)
{
  if (types.find(name) != types.end())
    throw ScribeException(makeString() << "'" << name << "' is already registered!");

  types.emplace(name, std::move(type));
}

void TypeRegistry::registerGeneric(std::unique_ptr<meta::GenericTypeDefinition>&& genericDefinition)
{
  generics.emplace(genericDefinition->getName(), std::move(genericDefinition));
}

const RegisterableTypeNotion& TypeRegistry::getType(const std::string& name) const
{
  return getType(TypeName(name));
}

const RegisterableTypeNotion& TypeRegistry::getType(const TypeName& name) const
{
  auto found = types.find(name.get());
  if (found == types.end())
    throw UnknownTypeError(makeString() << "Type '" << name.get() << "' is not registered!");

  return *(found->second);
}

const RegisterableTypeNotion& TypeRegistry::getSpecializedType(const TypeName& name, const Specialization& specialization) const
{
  const auto genericDefinition = generics.find(name);
  if (genericDefinition == generics.end())
    throw UnknownTypeError(makeString() << "Generic '" << name.get() << "' is not registered!");

  // TODO can we get the name here?
  const auto concreteDefinition = genericDefinition->second->specialize(specialization);

  const auto& concreteName = concreteDefinition.getName();
  const auto specialized = specializedTypes.find(concreteName);
  if (specialized != specializedTypes.end())
    return *(specialized->second);

  // [iterator, success]
  const auto emplaceResult = specializedTypes.emplace(concreteName,
      std::make_unique<types::ComplexTypeNotion>(concreteDefinition, *this));

  return *(emplaceResult.first->second);
}

void TypeRegistry::validate(const Entity& entity) const
{
  for (const auto& type : types)
  {
    try {
      type.second->validate(entity, ValidationContext{});
      return;
    }
    catch (const ScribeException& ex)
    {
      // it is just suppressed, it would be nicer if validate had a bool-returned version
    }
  }
  throw ScribeException(makeString() << "Validation failed for: " << printEntity(entity));
}

void TypeRegistry::validate(const Entity& entity, const ValidationContext& context) const
{
  if (!context.expected.get().empty())
  {
    try
    {
      const auto& expectedType = getType(context.expected);
      expectedType.validate(entity, context);
      return;
    }
    catch (const UnknownTypeError& ex)
    {
      throw TypeValidationError(makeString() << ex.what());
    }
    catch (const ScribeException& ex)
    {
      throw TypeValidationError(makeString() << "Expected '" << context.expected.get()
          << "'! Reason: '" << ex.what() << "', Got: '" << printEntity(entity) << "'");
    }
  }

  validate(entity);
}
