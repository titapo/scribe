#ifndef SCRIBE_TYPEREGISTRY_H_INCLUDED
#define SCRIBE_TYPEREGISTRY_H_INCLUDED

#include <scribe/TypeNotion.h>
#include <unordered_map>
#include <memory>

namespace scribe
{
  class TypeRegistry
  {
    public:
      using TypeName = std::string;

      void registerType(const TypeName& name, std::unique_ptr<TypeNotion>&& type);
      const TypeNotion& getType(const TypeName& name) const;

      // TODO validation context
      // TODO how to handle validation context (eg. type hint, expected type, skip meta, lazy/strict etc.)
      void validate(const Entity& entity) const;

    private:
      std::unordered_map<TypeName, std::unique_ptr<TypeNotion>> types;
  };
}
#endif
