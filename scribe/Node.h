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

            class iterator : public iterator_adaptor_to<container_type::iterator>
            {
                public:
                    using iterator_adaptor_to::iterator_adaptor_to;
                    inline WeakEntry operator*()
                    { return WeakEntry(orig->first, *(orig->second)); }
            };

            class const_iterator : public iterator_adaptor_to<container_type::const_iterator>
            {
                public:
                    using iterator_adaptor_to::iterator_adaptor_to;
                    inline WeakEntry operator*()
                    { return WeakEntry(orig->first, *(orig->second)); }
            };

            void addChild(OwnerEntry&& entry);
            void addChild(const std::string& name, std::unique_ptr<Entity> child);

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

            void processBy(EntityProcessor& processor) override;
            void processBy(EntityProcessor& processor) const override;
            void processBy(MutableEntityProcessor& processor) override;

        private:
            container_type children;
    };
}
#endif

