#ifndef SCRIBE_META_H_INCLUDED
#define SCRIBE_META_H_INCLUDED

#include <scribe/Node.h>
#include <scribe/std_polyfill.h> // for MetaConcept
#include <map>
#include <vector>

namespace scribe
{
  namespace meta
  {
    constexpr char metaSpecifier[] = "^^meta^^";
    constexpr char specifierKey[] = "?";
    using TypeName = std::string;

    class MetaException : public ScribeException
    {
      public:
        using ScribeException::ScribeException;
    };

    enum class MetaSpecifiers
    {
        TypeDefinition,
        TypeReference
    };

    // Meta concept
    template <typename T>
    using MetaConcept = std::void_t<
      decltype(std::declval<const T>().addToNode(std::declval<Node&>())),
      std::enable_if_t<
        std::is_same_v<decltype(T::fromNode(std::declval<const Node&>())), T>
      >
    >;

    template <typename T, typename = void>
    struct ImplementsMetaConcept : std::false_type {};

    template <typename T>
    struct ImplementsMetaConcept<T, MetaConcept<T>> : std::true_type {};

    class TypeDefinition
    {
        public:
          using TypeName = std::string;
          using GenericName = std::string;
          using Generics = std::vector<GenericName>;

          explicit TypeDefinition(const std::string& defName)
            : name(defName)
          {}

          struct Field
          {
            Field(const std::string& field, const TypeName& typeName)
              : name(field)
              , type(typeName)
            {}

            std::string name;
            TypeName type;
            // default/optional
          };

          using Fields = std::unordered_map<std::string, Field>;

          inline std::string getName() const
          { return name; }

          void addToNode(Node& node) const;

          void addField(const Field& field);
          void addField(Field&& field);
          const Fields& getFields() const
          {
            return fields;
          }

          void addGeneric(const GenericName& generic);

          const Generics& getGenerics() const
          { return generics; }

          TypeDefinition specialize(const std::vector<TypeName>& specializations) const; // TODO free function

          static TypeDefinition fromNode(const Node& node);

        private:
          std::string name;
          Generics generics;
          // inherits/parents
          Fields fields;
          // methods
    };

    static_assert(ImplementsMetaConcept<TypeDefinition>::value, "Expected MetaConcept implementation");

    class TypeReference
    {
        public:
          using TypeName = std::string;

          explicit TypeReference(const TypeName& typeName)
            : type(typeName)
          {}

          TypeName getTypename() const;

          void addToNode(Node& node) const;
          static TypeReference fromNode(const Node& node);

        private:
          const TypeName type;

    };

    static_assert(ImplementsMetaConcept<TypeReference>::value, "Expected MetaConcept implementation");
  }

}

#endif
