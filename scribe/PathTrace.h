#ifndef SCRIBE_PATHTRACE_H_INCLUDED
#define SCRIBE_PATHTRACE_H_INCLUDED

#include <scribe/common_helpers.h>
#include <scribe/PathElement.h>
#include <utility>
#include <vector>

namespace scribe
{
  class PathElement;

  class PathTrace
  {
    public:
      using element_type = std::pair<PathElement::Ptr, std::reference_wrapper<Entity>>;
      using weak_element_type = std::pair<PathElement&, std::reference_wrapper<Entity>>;
      using container_type = std::vector<element_type>;

      explicit PathTrace(Entity& entity);

      Entity& getLast() const;

      template <typename... Args>
      void emplace(Args&&... args)
      {
        elements.emplace_back(std::forward<Args>(args)...);
      }

      void extend(const PathElement& element);

      struct ToWeak
      {
          template <typename ValueType>
          weak_element_type operator()(const ValueType& value)
          { return {*(value.first), value.second}; }
      };

      using iterator = iterator_adaptor<container_type::iterator, ToWeak>;
      using const_iterator = iterator_adaptor<container_type::const_iterator, ToWeak>;

      inline iterator begin()
      { return iterator(elements.begin()); }

      inline const_iterator begin() const
      { return const_iterator(elements.begin()); }

      inline iterator end()
      { return iterator(elements.end()); }

      inline const_iterator end() const
      { return const_iterator(elements.end()); }

    private:
      std::reference_wrapper<Entity> root;
      container_type elements;
  };
}

#endif
