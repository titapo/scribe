#include <scribe/ComplexTypeNotion.h>
#include <scribe/makeString.h>
#include <scribe/TypeRegistry.h>

#include <algorithm>

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

        // FIXME it is not the best place for it
        static void createFieldsFromDefinition(types::ComplexTypeNotion::Fields& fields,
            const types::ComplexTypeNotion::TypeDefinition& definition,
            const TypeRegistry& registry)
        {
          auto createField = [&registry](const auto& field)
            {return Field{field.first, field.second.type.get(), registry.getType(field.second.type.get())};};

          std::transform(definition.getFields().begin(), definition.getFields().end(),
              std::back_inserter(fields), createField);
        }

        static auto hasSameName(const std::string& needle)
        {
          return [&needle](const auto& field)
          { return field.name == needle; };
        }

        static bool containsFieldWithName(const types::ComplexTypeNotion::Fields& fields, const std::string& needle)
        {
          return std::any_of(fields.begin(), fields.end(), hasSameName(needle));
        }

      private:
        void checkField(const types::ComplexTypeNotion::Field& field, const ValidationContext& context);
        void checkFieldType(const types::ComplexTypeNotion::Field& field, const ValidationContext& context);
        void checkExtraFields();

        const ComplexTypeNotion& outer;
        const Node& node;
    };
  }
}

using namespace scribe;


types::ComplexTypeNotion::ComplexTypeNotion(const TypeDefinition& def, const TypeRegistry& registry)
  : definition(def)
{
  Checker::createFieldsFromDefinition(fields, definition, registry);
}

types::ComplexTypeNotion::ComplexTypeNotion(TypeDefinition&& def, const TypeRegistry& registry)
  : definition(std::move(def))
{
  Checker::createFieldsFromDefinition(fields, definition, registry);
}


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
    throw TypeValidationError(makeString() << "'" << outer.definition.getName().get() << "' passed when '"
        << ref.getTypename().get() << "' was expected!");
}

void types::ComplexTypeNotion::Checker::checkFields(const ValidationContext& context)
{
  for (const auto& field : outer.fields)
    checkField(field, context);

  if (context.strictness == ValidationContext::Strictness::Strict)
    checkExtraFields();
}

void types::ComplexTypeNotion::Checker::checkExtraFields()
{
  for (const auto& child : node)
  {
    if (child.first == meta::metaSpecifier)
      continue;

    if (!containsFieldWithName(outer.fields, child.first))
      throw TypeValidationError(makeString() << "'" << outer.definition.getName().get() << "' should not have field named: '"
         << child.first << "'!");
  }
}

void types::ComplexTypeNotion::Checker::checkField(const types::ComplexTypeNotion::Field& field, const ValidationContext& context)
{
  if (!node.hasChild(field.name))
    throw TypeValidationError(makeString() << "'" << outer.definition.getName().get() << "' must have field: '"
        << field.name << "' (" << field.typeName << ")!");

  checkFieldType(field, context);
}

void types::ComplexTypeNotion::Checker::checkFieldType(
    const types::ComplexTypeNotion::Field& field,
    const ValidationContext& context)
{
  try
  {
    field.type.validate(node.getChild(field.name), context);
  }
  catch(const ScribeException& ex)
  {
    throw TypeValidationError(makeString()
        << "In " << outer.definition.getName().get()
        << " field '" << field.name << "' is not a(n) '" << field.typeName << "'! " << ex.what());
  }
}
