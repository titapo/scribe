#ifndef SCRIBE_TREE_H_INCLUDED
#define SCRIBE_TREE_H_INCLUDED

#include <scribe/Entity.h>
#include <scribe/Path.h>

namespace scribe
{
  class Tree
  {
    public:
      using Route = std::vector<std::string>;

      explicit Tree(std::unique_ptr<Entity>&& root);
      Tree(std::unique_ptr<Entity>&& root, Path&& path);

      Entity& getRoot() const;
      Entity& getCurrent() const;

      const PathTrace& getTrace() const;
      PathTrace& getTrace();

    private:
      std::unique_ptr<Entity> root;
      PathTrace current;
  };
}

#endif
