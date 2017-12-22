#ifndef SCRIBE_TYPEREGISTRY_H_INCLUDED
#define SCRIBE_TYPEREGISTRY_H_INCLUDED

#include <scribe/TypeNotion.h>
#include <unordered_map>
#include <memory>
#include <scribe/RegisterableTypeNotion.h>
#include <scribe/Meta.h> // FIXME TypeRegistry should not know Meta

namespace scribe
{
  class UnknownTypeError : public ScribeException
  {
    public:
      using ScribeException::ScribeException;
  };
  class TypeRegistry
  {
    public:
      using Specialization = std::vector<TypeName>; // init list?
      void registerType(const std::string& name, std::unique_ptr<TypeNotion>&& type);
      void registerType(const std::string& name, std::unique_ptr<RegisterableTypeNotion>&& type);
      void registerGeneric(std::unique_ptr<meta::GenericTypeDefinition>&& genericDefinition);
      const RegisterableTypeNotion& getType(const std::string& name) const;
      const RegisterableTypeNotion& getType(const TypeName& name) const;

      const RegisterableTypeNotion& getSpecializedType(const TypeName& name, const Specialization& specialization) const;

      // TODO validation context
      // TODO how to handle validation context (eg. type hint, expected type, skip meta, lazy/strict etc.)
      void validate(const Entity& entity) const;
      void validate(const Entity& entity, const ValidationContext& context) const;

    private:
      std::unordered_map<std::string, std::unique_ptr<RegisterableTypeNotion>> types;
      std::unordered_map<TypeName, std::unique_ptr<meta::GenericTypeDefinition>> generics;
      mutable std::unordered_map<TypeName, std::unique_ptr<RegisterableTypeNotion>> specializedTypes; // cache
  };
}
#endif
