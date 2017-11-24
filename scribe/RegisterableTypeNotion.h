#ifndef SCRIBE_REGISTERABLETYPENOTION_H_INCLUDED
#define SCRIBE_REGISTERABLETYPENOTION_H_INCLUDED

#include <scribe/exception.h>

namespace scribe
{
  class Entity;
  class TypeRegistry;

  struct ValidationContext
  {
    const TypeRegistry& registry; // TODO it should not be kept here
  };

  class TypeValidationError : public ScribeException
  {
    public:
      explicit TypeValidationError(const std::string& message);
      explicit TypeValidationError(const char* message);
  };

  // Helper class to be registered to contain simple TypeNotion and complex type notions
  class RegisterableTypeNotion
  {
    public:
      virtual void validate(const Entity& entity, const ValidationContext& context) const = 0;

      ~RegisterableTypeNotion() = default;
  };
}

#endif
