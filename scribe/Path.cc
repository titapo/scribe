#include <scribe/Path.h>
#include <sstream>

using namespace scribe;

void Path::extend(ElementPtr&& element)
{
  elements.push_back(std::move(element));
}

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

std::string Path::toString() const
{
  std::stringstream str;
  toStream(str);
  return str.str();
}

std::ostream& Path::toStream(std::ostream& ostr) const
{
  for (const auto& element : elements)
    ostr << *element;

  return ostr;
}
