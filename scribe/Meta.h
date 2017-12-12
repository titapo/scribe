#ifndef SCRIBE_META_H_INCLUDED
#define SCRIBE_META_H_INCLUDED

#include <scribe/Node.h>
#include <scribe/std_polyfill.h> // for MetaConcept
#include <scribe/TypeName.h>
#include <map>
#include <vector>

namespace scribe
{
  namespace meta
  {
    constexpr char metaSpecifier[] = "^^meta^^";
    constexpr char specifierKey[] = "?";

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

    class BasicTypeDefinition
    {
        public:
          explicit BasicTypeDefinition(const std::string& defName)
            : name(TypeName(defName))
          {}

          explicit BasicTypeDefinition(const TypeName& typeName)
            : name(typeName)
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

          void addToNode(Node& node) const;

          inline TypeName getName() const
          { return name; }

          void addField(const Field& field);
          void addField(Field&& field);
          const Fields& getFields() const
          {
            return fields;
          }
        private:
          TypeName name;
          // inherits/parents
          Fields fields;
          // methods
    };

    class TypeDefinition : public BasicTypeDefinition
    {
      public:
        using BasicTypeDefinition::BasicTypeDefinition;

        void addToNode(Node& node) const;
        static TypeDefinition fromNode(const Node& node);
    };
    static_assert(ImplementsMetaConcept<TypeDefinition>::value, "Expected MetaConcept implementation");

    class GenericTypeDefinition : public BasicTypeDefinition
    {
      public:
        using GenericName = std::string;
        using Generics = std::vector<GenericName>;

        GenericTypeDefinition(const TypeName& typeName, Generics&& genericList);
        GenericTypeDefinition(TypeName&& typeName, Generics&& genericList);

        //void addGeneric(const GenericName& generic);
        const Generics& getGenerics() const;

        void addToNode(Node& node) const;
        static GenericTypeDefinition fromNode(const Node& node);

        TypeDefinition specialize(const std::vector<TypeName>& specializations) const; // TODO free function

      private:
        /*const*/Generics generics;
    };


    class TypeReference
    {
        public:
          explicit TypeReference(const std::string& typeName)
            : type(TypeName(typeName))
          {}

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
