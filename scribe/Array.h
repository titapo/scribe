#ifndef SCRIBE_ARRAY_H_INCLUDED
#define SCRIBE_ARRAY_H_INCLUDED

#include <cstddef>
#include <vector>

#include <scribe/common_helpers.h>
#include <scribe/Entity.h>

namespace scribe
{
    class Array : public Entity
    {
        public:
            using Index = unsigned;
            using container_type = std::vector<std::unique_ptr<Entity>>;

            void processBy(EntityProcessor& processor) override;
            void processBy(EntityProcessor& processor) const override;
            void processBy(MutableEntityProcessor& processor) override;

            std::size_t size() const;

            class iterator : public iterator_adaptor_to<container_type::iterator>
            {
                public:
                    using iterator_adaptor_to::iterator_adaptor_to;
                    inline Entity& operator*()
                    { return *(orig->get()); }
            };

            class const_iterator : public iterator_adaptor_to<container_type::const_iterator>
            {
                public:
                    using iterator_adaptor_to::iterator_adaptor_to;
                    inline Entity& operator*()
                    { return *(orig->get()); }
            };

            inline const_iterator begin() const { return const_iterator(children.begin()); }
            inline const_iterator end() const { return const_iterator(children.end()); }

            Entity& getChild(Index index) const;
            void append(std::unique_ptr<Entity> child);
        private:
            container_type children;
    };
}

#endif
