#ifndef SCRIBE_TYPENOTION_H_INCLUDED
#define SCRIBE_TYPENOTION_H_INCLUDED

#include <scribe/Entity.h>
#include <scribe/EntityProcessor.h> // only for LeafValidator
#include <scribe/exception.h>
#include <scribe/EntityProcessor.h>
#include <scribe/Leaf.h>

namespace scribe
{
  class TypeNotion
  {
    public:
      virtual void validate(const Entity& entity) const = 0;
      virtual std::unique_ptr<Entity> instantiate() = 0;
        
      virtual ~TypeNotion() {}
  };

  namespace types
  {
    class Any : public TypeNotion
    {
      public:
        void validate(const Entity& entity) const override;
        std::unique_ptr<Entity> instantiate() override;
    };

    class NodeType : public TypeNotion
    {
      public:
        void validate(const Entity& entity) const override;
        std::unique_ptr<Entity> instantiate() override;
        Node& get(Entity&) const;
        const Node& get(const Entity&) const;
    };

    class ArrayType : public TypeNotion
    {
      public:
        void validate(const Entity& entity) const override;
        std::unique_ptr<Entity> instantiate() override;
        Array& get(Entity&) const;
        const Array& get(const Entity&) const;
    };

    class LeafBaseValidator : public EntityProcessor
    {
      void process(const LeafBase&) override { }
      void process(const Array&) override { throw ScribeException("not a Leaf"); }
      void process(const Node&) override { throw ScribeException("not a Leaf"); }
    };

    class LeafBaseType : public TypeNotion
    {
      public:
        void validate(const Entity& entity) const override;
        std::unique_ptr<Entity> instantiate() override;
    };

    template <typename T>
    class LeafValidator : public LeafBaseValidator
    {
      void process(const LeafBase& leaf) override
      {
        if (!dynamic_cast<const Leaf<T>*>(&leaf))
          throw ScribeException("Not a proper leaf!");
      }
    };

    template <typename T>
    class LeafType : public TypeNotion
    {
      public:
        void validate(const Entity& entity) const override
        {
          LeafValidator<T> validator;
          entity.processBy(validator);
        }

        std::unique_ptr<Entity> instantiate() override
        {
          return Entity::create<Leaf<T>>(T{});
        }

        Leaf<T>& get(Entity& entity) const
        {
          auto leaf = dynamic_cast<Leaf<T>*>(&entity);
          if (!leaf)
            throw ScribeException("Not a proper leaf!");

          return *leaf;
        }

        const Leaf<T>& get(const Entity& entity) const
        {
          const auto leaf = dynamic_cast<const Leaf<T>*>(&entity);
          if (!leaf)
            throw ScribeException("Not a proper leaf!");

          return *leaf;
        }
    };
  }
}
#endif
