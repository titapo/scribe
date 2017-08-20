#ifndef SCRIBE_ARRAY_H_INCLUDED
#define SCRIBE_ARRAY_H_INCLUDED

#include <cstddef>
#include <vector>

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
                    inline Entity& operator*()
                    { return *(orig->get()); }

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
                    inline Entity& operator*()
                    { return *(orig->get()); }

                    orig_const_iterator_type orig;
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
