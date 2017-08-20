#ifndef SCRIBE_NODE_H_INCLUDED
#define SCRIBE_NODE_H_INCLUDED

#include <cstddef>
#include <string>
#include <memory>
#include <unordered_map>

#include <scribe/Entity.h>
#include <scribe/exception.h>


class EntityProcessor;

namespace scribe
{

    class NoSuchChild : public ScribeException
    {
        public:
            using ScribeException::ScribeException;
    };


    class Node : public Entity
    {
        public:
            using OwnerEntry = std::pair<std::string, std::unique_ptr<Entity>>;
            using WeakEntry = std::pair<std::string, Entity&>;
            using container_type = std::unordered_map<std::string, std::unique_ptr<Entity>>;

            std::size_t size() const;

            class iterator
            {
                public:
                    using orig_iterator_type = typename container_type::iterator;

                    iterator(orig_iterator_type orig);

                    inline iterator& operator++()
                    {
                        orig++;
                        return *this;
                    }

                    inline bool operator==(const iterator& rhs) const
                    { return orig == rhs.orig; }
                    inline bool operator!=(const iterator& rhs) const
                    { return orig != rhs.orig; }
                    inline WeakEntry operator*()
                    { return WeakEntry(orig->first, *(orig->second)); }

                    orig_iterator_type orig;
            };

            class const_iterator
            {
                public:
                    using orig_const_iterator_type = typename container_type::const_iterator;

                    const_iterator(orig_const_iterator_type orig);

                    inline const_iterator& operator++()
                    {
                        orig++;
                        return *this;
                    }
                    inline bool operator==(const const_iterator& rhs) const
                    { return orig == rhs.orig; }
                    inline bool operator!=(const const_iterator& rhs) const
                    { return orig != rhs.orig; }
                    inline WeakEntry operator*()
                    { return WeakEntry(orig->first, *(orig->second)); }

                    orig_const_iterator_type orig;
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

            Entity& getChild(const std::string& name) const;
            WeakEntry getEntry(const std::string& name) const;

            void processBy(EntityProcessor& processor) override;
            void processBy(EntityProcessor& processor) const override;

        private:
            container_type children;
    };
}
#endif

