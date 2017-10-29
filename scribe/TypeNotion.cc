#include <scribe/TypeNotion.h>
#include <scribe/Node.h>
#include <scribe/Array.h>

using namespace scribe;


namespace
{
  template <typename Type, typename Base>
  using IsBaseOf = typename std::enable_if<std::is_base_of<EntityProcessor, Base>::value>::type;

  template <
    typename Type, typename Base,
    typename = IsBaseOf<Type, Base>>
  class Provider : public Base
  {
    public:
      void process(const Type& t) override
      { stored = &t; }

      const Type& get() const
      { return *stored; }

    private:
      const Type* stored{nullptr};
  };
}

void types::Any::validate(const Entity&) const
{
  return;
}

std::unique_ptr<Entity> types::Any::instantiate()
{
  throw ScribeException("'Any' cannot be instantiated!");
}

namespace
{

  class NodeValidator : public EntityProcessor
  {
    void process(const Node&) override {}
    void process(const LeafBase&) override { throw ScribeException("not a Node"); }
    void process(const Array&) override { throw ScribeException("not a Node"); }
  };

  using NodeProvider = Provider<Node, NodeValidator>;
}

void types::NodeType::validate(const Entity& entity) const
{
  NodeValidator validator;
  entity.processBy(validator);
}

const Node& types::NodeType::get(const Entity& entity) const
{
  NodeProvider provider;
  entity.processBy(provider);
  return provider.get();
}

std::unique_ptr<Entity> types::NodeType::instantiate()
{
  return Entity::create<Node>();
}

namespace
{
  class ArrayValidator : public EntityProcessor
  {
    void process(const Array&) override
    {}
    void process(const LeafBase&) override
    { throw ScribeException("not an Array"); }
    void process(const Node&) override
    { throw ScribeException("not an Array"); }
  };

  using ArrayProvider = Provider<Array, ArrayValidator>;
}

void types::ArrayType::validate(const Entity& entity) const
{
  ArrayValidator validator;
  entity.processBy(validator);
}

std::unique_ptr<Entity> types::ArrayType::instantiate()
{
  return Entity::create<Array>();
}

void types::LeafBaseType::validate(const Entity& entity) const
{
  LeafBaseValidator validator;
  entity.processBy(validator);
}

std::unique_ptr<Entity> types::LeafBaseType::instantiate()
{
  throw ScribeException("'Leaf' cannot be instantiated!");
}

