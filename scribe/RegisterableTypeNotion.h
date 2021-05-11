#ifndef SCRIBE_REGISTERABLETYPENOTION_H_INCLUDED
#define SCRIBE_REGISTERABLETYPENOTION_H_INCLUDED

#include <scribe/exception.h>
#include <scribe/TypeName.h>
#include <memory>

namespace scribe
{
  class Entity;
  class TypeRegistry;

  struct ValidationContext
  {
    enum class Strictness
    {
      Strict,
      Lazy
    };

    ValidationContext() = default;

    explicit ValidationContext(Strictness strict)
      : strictness(strict)
    {}

    explicit ValidationContext(const TypeName& expectedType)
      : expected(expectedType)
    {}

    Strictness strictness = Strictness::Strict;
    TypeName expected{""}; // TODO optional
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
      virtual std::unique_ptr<Entity> instantiate() const = 0;

      ~RegisterableTypeNotion() = default;
  };
}

#endif
