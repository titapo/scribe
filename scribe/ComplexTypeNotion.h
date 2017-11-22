#ifndef SCRIBE_COMPLEXTYPENOTION_H_INCLUDED
#define SCRIBE_COMPLEXTYPENOTION_H_INCLUDED

#include <scribe/TypeNotion.h>
#include <scribe/Meta.h>
#include <scribe/RegisterableTypeNotion.h>

namespace scribe
{
  namespace types
  {
    class ComplexTypeNotion : public RegisterableTypeNotion
    {
      public:
        using TypeDefinition = meta::TypeDefinition;

        explicit ComplexTypeNotion(const TypeDefinition& definition);
        explicit ComplexTypeNotion(TypeDefinition&& definition);
        void validate(const Entity& entity, const ValidationContext& context) const override;

      private:

        class Checker;
        TypeDefinition definition; // TODO it should not be stored by value
        NodeType nodeType;
    };
  }
}


#endif
