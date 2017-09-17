#include <sstream>
#include <scribe/Path.h>
#include <scribe/Entity.h>
#include <scribe/Node.h>
#include <scribe/Array.h>
#include <scribe/EntityProcessor.h>

using namespace scribe;

namespace
{
  class PathElementProcessor : public scribe::EntityProcessor
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
      ChildGetterProcessor(const std::string& name)
        : name(name)
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
      IndexGetterProcessor(Path::Index::index_t index)
        : index(index)
      {}

      void process(const Array& array) override
      {
        result = &array.getChild(index);
      }

    private:
      const Path::Index::index_t index;
  };
}

std::string Path::Element::toString() const
{
  std::stringstream str;
  toStream(str);
  return str.str();
}

Entity& Path::Child::evaluate(Entity& entity) const
{
  ChildGetterProcessor processor(name);
  entity.processBy(processor);
  return processor.getResult();
}
std::ostream& Path::Child::toStream(std::ostream& str) const
{ return str << "." << name;}

Entity& Path::Index::evaluate(Entity& entity) const
{
  IndexGetterProcessor processor(index);
  entity.processBy(processor);
  return processor.getResult();
}

std::ostream& Path::Index::toStream(std::ostream& str) const
{ return str << "[" << index << "]";}

PathTrace Path::evaluateTrace(Entity& entity) const
{
  PathTrace trace(entity);
  for (const auto& element : elements)
  {
    trace.extend(*element);
  }
  return trace;
}

Entity& Path::evaluate(Entity& entity) const
{
  return evaluateTrace(entity).getLast();
}

Path::Path(Elements&& newElements)
{
  for (auto&& e : newElements)
    elements.push_back(std::move(e));
}

Path::Child::Child(const std::string& name)
  : name(name)
{}

Path::Index::Index(Index::index_t index)
  : index(index)
{}


PathTrace::PathTrace(Entity& entity)
  : root(entity)
{}

Entity& PathTrace::getLast() const
{
  if (elements.empty())
    return root;

  return elements.back().second;
}

void PathTrace::extend(const Path::Element& element)
{
  emplace(element.clonePtr(), element.evaluate(getLast()));
}
