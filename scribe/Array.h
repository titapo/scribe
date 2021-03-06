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

            struct ToEntity
            {
                template <typename ValueType>
                Entity& operator()(const ValueType& value)
                { return *(value.get()); }
            };

            using iterator = iterator_adaptor<container_type::iterator, ToEntity>;
            using const_iterator = iterator_adaptor<container_type::const_iterator, ToEntity>;

            inline const_iterator begin() const { return const_iterator(children.begin()); }
            inline const_iterator end() const { return const_iterator(children.end()); }

            Entity& getChild(Index index) const;
            void append(std::unique_ptr<Entity> child);
        private:
            container_type children;
    };
}

#endif
