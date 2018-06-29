#include <scribe/Tree.h>

using namespace scribe;

Tree::Tree(std::unique_ptr<Entity>&& aRoot)
  : root(std::move(aRoot))
  , current(*root)
{}

Tree::Tree(std::unique_ptr<Entity>&& aRoot, Path&& path)
  : root(std::move(aRoot))
  , current(path.evaluateTrace(*root))
{}

Entity& Tree::getRoot() const
{
  return *root;
}

Entity& Tree::getCurrent() const
{
  return current.getLast();
}
