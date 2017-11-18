#ifndef SCRIBE_COMPLEXTYPENOTION_H_INCLUDED
#define SCRIBE_COMPLEXTYPENOTION_H_INCLUDED

#include <scribe/TypeNotion.h>
#include <scribe/Meta.h>

namespace scribe
{
  namespace types
  {
    class ComplexTypeNotion : public TypeNotion // NodeType??
    {
      public:
        using TypeDefinition = meta::TypeDefinition;

        explicit ComplexTypeNotion(const TypeDefinition& definition);
        explicit ComplexTypeNotion(TypeDefinition&& definition);
        void validate(const Entity& entity) const override;
        std::unique_ptr<Entity> instantiate() override;

      private:
        TypeDefinition definition; // TODO it should not be stored by value
        NodeType nodeType;
    };
  }
}


#endif
