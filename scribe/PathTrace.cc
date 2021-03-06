#include <scribe/PathTrace.h>

using namespace scribe;

PathTrace::PathTrace(Entity& entity)
  : root(entity)
{}

Entity& PathTrace::getLast() const
{
  if (elements.empty())
    return root;

  return elements.back().second;
}

void PathTrace::extend(const PathElement& element)
{
  emplace(element.clonePtr(), element.evaluate(getLast()));
}

void PathTrace::drop()
{
  if (!elements.empty())
    elements.pop_back();
}

void PathTrace::reset()
{
  elements.clear();
}
