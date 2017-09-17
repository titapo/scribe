#ifndef SCRIBE_PATH_H_INCLUDED
#define SCRIBE_PATH_H_INCLUDED

#include <string>
#include <vector>
#include <memory>
#include <scribe/common_helpers.h> // for PathTrace

namespace scribe
{
  class Entity;
  class PathTrace;

  class Path
  {
    public:
      class Element
      {
        public:
          using Ptr = std::unique_ptr<Element>;
          virtual Entity& evaluate(Entity& entity) const = 0;
          virtual std::unique_ptr<Element> clonePtr() const = 0;
          virtual std::ostream& toStream(std::ostream& str) const = 0;
          std::string toString() const;

          virtual ~Element() = default;
      };

      using ElementPtr = Element::Ptr;
      using Elements = std::vector<ElementPtr>;

      Path(Elements&& newElements);

      Entity& evaluate(Entity& entity) const;
      PathTrace evaluateTrace(Entity& entity) const;

      class Child : public Element
      {
        public:
          Child(const std::string& name);

          virtual std::unique_ptr<Element> clonePtr() const override
          { return ElementPtr(new Child(name)); }

          std::ostream& toStream(std::ostream& str) const override;

          Entity& evaluate(Entity& entity) const override;

        private:
          const std::string name;
      };

      class Index : public Element
      {
        public:
          using index_t = unsigned;
          Index(index_t index);

          virtual std::unique_ptr<Element> clonePtr() const override
          { return ElementPtr(new Index(index)); }

          std::ostream& toStream(std::ostream& str) const override;

          Entity& evaluate(Entity& entity) const override;

        private:
          const index_t index;
      };

    private:
      Elements elements;
  };

  class PathTrace
  {
    public:
      using element_type = std::pair<Path::ElementPtr, std::reference_wrapper<Entity>>;
      using weak_element_type = std::pair<Path::Element&, std::reference_wrapper<Entity>>;
      using container_type = std::vector<element_type>;

      PathTrace(Entity& entity);

      Entity& getLast() const;

      template <typename... Args>
        void emplace(Args&&... args)
        {
          elements.emplace_back(std::forward<Args>(args)...);
        }

      void extend(const Path::Element& element);

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

  inline std::ostream& operator<<(std::ostream& str, const Path::Element& element)
  {
    return element.toStream(str);
  }
}

#endif
