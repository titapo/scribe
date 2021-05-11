#ifndef SCRIBE_NODE_H_INCLUDED
#define SCRIBE_NODE_H_INCLUDED

#include <cstddef>
#include <string>
#include <memory>
#include <unordered_map>

#include <scribe/Entity.h>
#include <scribe/exception.h>
#include <scribe/common_helpers.h>


namespace scribe
{

    class EntityProcessor;

    class Node : public Entity
    {
        public:
            using OwnerEntry = std::pair<std::string, std::unique_ptr<Entity>>;
            using WeakEntry = std::pair<std::string, Entity&>;
            using container_type = std::unordered_map<std::string, std::unique_ptr<Entity>>;

            std::size_t size() const;

            struct ToWeakEntry
            {
                template <typename ValueType>
                WeakEntry operator()(const ValueType& value)
                { return WeakEntry(value.first, *(value.second)); }
            };

            using iterator = iterator_adaptor<container_type::iterator, ToWeakEntry>;
            using const_iterator = iterator_adaptor<container_type::const_iterator, ToWeakEntry>;

            void addChild(OwnerEntry&& entry);
            void addChild(const std::string& name, std::unique_ptr<Entity> child);
            std::unique_ptr<Entity> setChild(const std::string& name, std::unique_ptr<Entity> child);

            inline iterator begin()
            { return iterator(children.begin()); }

            inline const_iterator begin() const
            { return const_iterator(children.begin()); }

            inline iterator end()
            { return iterator(children.end()); }

            inline const_iterator end() const
            { return const_iterator(children.end()); }

            bool hasChild(const std::string& name) const;
            Entity& getChild(const std::string& name) const;
            WeakEntry getEntry(const std::string& name) const;

            std::unique_ptr<Entity> removeChild(const std::string& name);

            void processBy(EntityProcessor& processor) override;
            void processBy(EntityProcessor& processor) const override;
            void processBy(MutableEntityProcessor& processor) override;

        private:
            container_type children;
    };
}
#endif

