#include <scribe/TypeNotion.h>
#include <scribe/Node.h>
#include <scribe/Array.h>

using namespace scribe;

void types::Any::validate(const Entity&)
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
}

void types::NodeType::validate(const Entity& entity)
{
  NodeValidator validator;
  entity.processBy(validator);
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
}

void types::ArrayType::validate(const Entity& entity)
{
  ArrayValidator validator;
  entity.processBy(validator);
}

std::unique_ptr<Entity> types::ArrayType::instantiate()
{
  return Entity::create<Array>();
}

void types::LeafBaseType::validate(const Entity& entity)
{
  LeafBaseValidator validator;
  entity.processBy(validator);
}

std::unique_ptr<Entity> types::LeafBaseType::instantiate()
{
  throw ScribeException("'Leaf' cannot be instantiated!");
}

