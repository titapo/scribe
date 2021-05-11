#ifndef SCRIBE_COMPLEXTYPENOTION_H_INCLUDED
#define SCRIBE_COMPLEXTYPENOTION_H_INCLUDED

#include <scribe/TypeNotion.h>
#include <scribe/Meta.h>
#include <scribe/RegisterableTypeNotion.h>
#include <vector>

namespace scribe
{
  namespace types
  {
    // This class stores references to typenotions of fields described in definition
    // So lifetime of TypeRegistry must be longer than ComplexTypeNotion instances
    class ComplexTypeNotion : public RegisterableTypeNotion, NonTransferable
    {
      public:
        using TypeDefinition = meta::TypeDefinition;

        explicit ComplexTypeNotion(const TypeDefinition& definition, const TypeRegistry& registry);
        void validate(const Entity& entity, const ValidationContext& context) const override;
        const TypeName& getName() const;
        std::unique_ptr<Entity> instantiate() const override;

      private:
        class Checker;

        struct Field
        {
          using TypeName = std::string;
          const std::string name;
          const TypeName typeName;
          const RegisterableTypeNotion& type;
        };
        using Fields = std::vector<Field>;

        TypeName name;
        NodeType nodeType;
        Fields fields;
    };
  }
}


#endif
