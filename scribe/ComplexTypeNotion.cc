#include <scribe/ComplexTypeNotion.h>
#include <scribe/makeString.h>
#include <scribe/TypeRegistry.h>

namespace scribe
{
  namespace types
  {
    class ComplexTypeNotion::Checker
    {
      public:
        Checker(const ComplexTypeNotion& notion, const Node& nodeToCheck)
          : outer(notion)
          , node(nodeToCheck)
        {}

        void checkReference();
        void checkFields(const ValidationContext& context);

      private:
        void checkField(const meta::TypeDefinition::Field& field, const ValidationContext& context);
        void checkFieldType(const meta::TypeDefinition::Field& field, const ValidationContext& context);

        const ComplexTypeNotion& outer;
        const Node& node;
    };
  }
}

using namespace scribe;

types::ComplexTypeNotion::ComplexTypeNotion(const TypeDefinition& def)
  : definition(def)
{}

types::ComplexTypeNotion::ComplexTypeNotion(TypeDefinition&& def)
  : definition(std::move(def))
{}


void types::ComplexTypeNotion::validate(const Entity& entity, const ValidationContext& context) const
{
  nodeType.validate(entity);
  try
  {
    const auto& node = nodeType.get(entity);

    Checker checker(*this, node);
    checker.checkReference();
    checker.checkFields(context);

  }
  catch (const meta::MetaException& ex)
  {
    throw;
  }
}

void types::ComplexTypeNotion::Checker::checkReference()
{
  const auto& ref = meta::TypeReference::fromNode(node);
  if (ref.getTypename() != outer.definition.getName())
    throw TypeValidationError(makeString() << "'" << outer.definition.getName() << "' passed when '"
        << ref.getTypename() << "' was expected!");
}

void types::ComplexTypeNotion::Checker::checkFields(const ValidationContext& context)
{
  for (const auto& field : outer.definition.getFields())
    checkField(field.second, context);
}

void types::ComplexTypeNotion::Checker::checkField(const meta::TypeDefinition::Field& field, const ValidationContext& context)
{
  if (!node.hasChild(field.name))
    throw TypeValidationError(makeString() << "'" << outer.definition.getName() << "' must have field: '"
        << field.name << "' (" << field.type << ")!");

  checkFieldType(field, context);
}

void types::ComplexTypeNotion::Checker::checkFieldType(
    const meta::TypeDefinition::Field& field,
    const ValidationContext& context)
{
  const auto& fieldNotion = context.registry.getType(field.type);
  try
  {
    fieldNotion.validate(node.getChild(field.name), context);
  }
  catch(const ScribeException& ex)
  {
    throw TypeValidationError(makeString()
        << "In " << outer.definition.getName()
        << " field '" << field.name << "' is not a(n) '" << field.type << "'! " << ex.what());
  }
}
