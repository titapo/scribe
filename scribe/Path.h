#ifndef SCRIBE_PATH_H_INCLUDED
#define SCRIBE_PATH_H_INCLUDED

#include <string>
#include <vector>
#include <memory>
#include <scribe/PathElement.h>
#include <scribe/PathTrace.h>

namespace scribe
{
  class Entity;
  class PathTrace;

  class Path
  {
    public:
      using ElementPtr = PathElement::Ptr;
      using Elements = std::vector<ElementPtr>;
      using iterator = typename Elements::iterator;
      using const_iterator = typename Elements::const_iterator;

      explicit Path(Elements&& newElements);

      Path() = default;
      void extend(ElementPtr&& element);

      Entity& evaluate(Entity& entity) const;
      PathTrace evaluateTrace(Entity& entity) const;

      std::string toString() const;
      std::ostream& toStream(std::ostream& ostr) const;

      inline iterator begin()
      { return elements.begin(); }

      inline const_iterator begin() const
      { return elements.begin(); }

      inline iterator end()
      { return elements.end(); }

      inline const_iterator end() const
      { return elements.end(); }

    private:
      Elements elements;
  };
  inline std::ostream& operator<<(std::ostream& str, const Path& path)
  {
    return path.toStream(str);
  }
}

#endif
