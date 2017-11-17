#include <scribe/Array.h>
#include <scribe/Entity.h>
#include <scribe/EntityProcessor.h>
#include <scribe/Node.h>
#include <scribe/PathElement.h>

#include <sstream>

using namespace scribe;

namespace
{
  class PathElementProcessor : public EntityProcessor
  {
    public:
      void process(const LeafBase&) override
      { }
      void process(const Node&) override
      { }
      void process(const Array&) override
      { }

      Entity& getResult() const
      {
        if (!result)
          throw ScribeException("Cannot get result!");

        return *result;
      }

    protected:
      Entity* result = nullptr;
  };

  class ChildGetterProcessor : public PathElementProcessor
  {
    public:
      ChildGetterProcessor(const std::string& key)
        : name(key)
      {}

      void process(const Node& node) override
      {
        result = &node.getChild(name);
      }

    private:
      const std::string& name;
  };

  class IndexGetterProcessor : public PathElementProcessor
  {
    public:
      IndexGetterProcessor(Index::index_t indexToFind)
        : index(indexToFind)
      {}

      void process(const Array& array) override
      {
        result = &array.getChild(index);
      }

    private:
      const Index::index_t index;
  };
}

std::string PathElement::toString() const
{
  std::stringstream str;
  toStream(str);
  return str.str();
}

Child::Child(const std::string& keyToFind)
  : name(keyToFind)
{}

Entity& Child::evaluate(Entity& entity) const
{
  ChildGetterProcessor processor(name);
  entity.processBy(processor);
  return processor.getResult();
}

std::ostream& Child::toStream(std::ostream& str) const
{ return str << "." << name;}

Index::Index(Index::index_t indexToFind)
  : index(indexToFind)
{}

Entity& Index::evaluate(Entity& entity) const
{
  IndexGetterProcessor processor(index);
  entity.processBy(processor);
  return processor.getResult();
}

std::ostream& Index::toStream(std::ostream& str) const
{ return str << "[" << index << "]";}

