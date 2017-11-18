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

      explicit Path(Elements&& newElements);

      Path() = default;
      void extend(ElementPtr&& element);

      Entity& evaluate(Entity& entity) const;
      PathTrace evaluateTrace(Entity& entity) const;

      std::string toString() const;
      std::ostream& toStream(std::ostream& ostr) const;

    private:
      Elements elements;
  };
  inline std::ostream& operator<<(std::ostream& str, const Path& path)
  {
    return path.toStream(str);
  }
}

#endif
