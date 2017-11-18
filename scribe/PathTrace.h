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

      class iterator : public iterator_adaptor_to<container_type::iterator>
      {
        public:
          using iterator_adaptor_to::iterator_adaptor_to;
          inline weak_element_type operator*()
          { return weak_element_type(*(orig->first), orig->second); }
      };

      class const_iterator : public iterator_adaptor_to<container_type::const_iterator>
      {
        public:
          using iterator_adaptor_to::iterator_adaptor_to;
          inline weak_element_type operator*()
          { return weak_element_type(*(orig->first), orig->second); }
      };

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
