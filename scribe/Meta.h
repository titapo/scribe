#ifndef SCRIBE_META_H_INCLUDED
#define SCRIBE_META_H_INCLUDED

#include <scribe/Node.h>
#include <map>

namespace scribe
{
  namespace meta
  {

    constexpr char metaSpecifier[] = "^^meta^^";
    constexpr char specifierKey[] = "?";

    enum class MetaSpecifiers
    {
        TypeDefinition
        //TypeReference,
    };

    class MetaBase
    {
        public:
            virtual void addToNode(Node& node) const = 0;
            virtual ~MetaBase() {}
    };

    class TypeDefinition : public MetaBase
    {
        public:
          using TypeName = std::string;

          TypeDefinition(const std::string& name)
            : name(name)
          {}

          struct Field
          {
            Field(const std::string& name, const TypeName& type)
              : name(name)
              , type(type)
            {}

            std::string name;
            TypeName type;
            // default/optional
          };

          using Fields = std::unordered_map<std::string, Field>;

          inline std::string getName() const
          { return name; }

          void addToNode(Node& node) const override;
          void addField(Field&& field);

          const Fields& getFields() const
          {
            return fields;
          }

          static TypeDefinition fromNode(const Node& node);

        private:
          std::string name;
          // inherits/parents
          Fields fields;
          // methods

    };

  }

}

#endif