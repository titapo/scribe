#ifndef SCRIBE_PATHELEMENT_H_INCLUDED
#define SCRIBE_PATHELEMENT_H_INCLUDED

#include <scribe/PathElement.h>
#include <scribe/Entity.h>
#include <memory>

namespace scribe
{
  class PathElement
  {
    public:
      using Ptr = std::unique_ptr<PathElement>;
      virtual Entity& evaluate(Entity& entity) const = 0;
      virtual std::unique_ptr<PathElement> clonePtr() const = 0;
      virtual std::ostream& toStream(std::ostream& str) const = 0;
      std::string toString() const;

      virtual ~PathElement() = default;
  };

  inline std::ostream& operator<<(std::ostream& str, const PathElement& element)
  {
    return element.toStream(str);
  }

  class Child : public PathElement
  {
    public:
      explicit Child(const std::string& name);

      PathElement::Ptr clonePtr() const override
      { return PathElement::Ptr(new Child(name)); }

      std::ostream& toStream(std::ostream& str) const override;

      Entity& evaluate(Entity& entity) const override;

    private:
      const std::string name;
  };

  class Index : public PathElement
  {
    public:
      using index_t = unsigned;
      explicit Index(index_t index);

      PathElement::Ptr clonePtr() const override
      { return PathElement::Ptr(new Index(index)); }

      std::ostream& toStream(std::ostream& str) const override;

      Entity& evaluate(Entity& entity) const override;

    private:
      const index_t index;
  };

}

#endif
