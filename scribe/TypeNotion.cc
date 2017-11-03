#include <scribe/TypeNotion.h>
#include <scribe/Node.h>
#include <scribe/Array.h>

using namespace scribe;

namespace
{
  template <typename Type, typename Base>
  class Provider : public Base
  {
    public:
      using stored_type = Type;

      void process(Type& t) override
      { stored = &t; }

      Type& get() const
      { return *stored; }

    private:
      Type* stored{nullptr};
  };

  template <bool C, typename T>
  using add_const_if = typename std::conditional<C, std::add_const<T>, T>::type;

  template <typename T, typename U>
  using forward_constness = add_const_if<std::is_const<T>::value, U>;

  template <typename provider_type,
            typename in_type = forward_constness<typename provider_type::stored_type, Entity>
              >
  auto& generic_get(in_type& entity)
  {
    provider_type provider;
    entity.processBy(provider);
    return provider.get();
  }
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

  class MutableNodeValidator : public MutableEntityProcessor
  {
    void process(Node&) override {}
    void process(LeafBase&) override { throw ScribeException("not a Node"); }
    void process(Array&) override { throw ScribeException("not a Node"); }
  };

  using NodeProvider = Provider<const Node, NodeValidator>;
  using MutableNodeProvider = Provider<Node, MutableNodeValidator>;
}

void types::NodeType::validate(const Entity& entity) const
{
  NodeValidator validator;
  entity.processBy(validator);
}

const Node& types::NodeType::get(const Entity& entity) const
{
  return generic_get<NodeProvider>(entity);
}

Node& types::NodeType::get(Entity& entity) const
{
  return generic_get<MutableNodeProvider>(entity);
}

std::unique_ptr<Entity> types::NodeType::instantiate()
{
  return Entity::create<Node>();
}

namespace
{
  class ArrayValidator : public EntityProcessor
  {
    void process(const Array&) override {}
    void process(const LeafBase&) override { throw ScribeException("not an Array"); }
    void process(const Node&) override { throw ScribeException("not an Array"); }
  };

  class MutableArrayValidator : public MutableEntityProcessor
  {
    void process(Array&) override {}
    void process(LeafBase&) override { throw ScribeException("not an Array"); }
    void process(Node&) override { throw ScribeException("not an Array"); }
  };

  using ArrayProvider = Provider<const Array, ArrayValidator>;
  using MutableArrayProvider = Provider<Array, MutableArrayValidator>;
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

const Array& types::ArrayType::get(const Entity& entity) const
{
  return generic_get<ArrayProvider>(entity);
}

Array& types::ArrayType::get(Entity& entity) const
{
  return generic_get<MutableArrayProvider>(entity);
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

