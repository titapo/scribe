#ifndef SCRIBE_LEAF_H_INCLUDED
#define SCRIBE_LEAF_H_INCLUDED

#include <scribe/Entity.h>

namespace scribe
{
    template <typename T>
    class Leaf : public Entity
    {
        public:
            using value_type = T;
            Leaf(const value_type& initial)
                : value(initial)
            {}

            Leaf(value_type&& initial)
                : value(std::move(initial))
            {}

            void setValue(value_type&& newValue)
            {
                value = std::move(newValue);
            }

            value_type getValue() const
            {
                return value;
            }

            void processBy(EntityProcessor& processor) override
            {
                (void)processor;
            }
            void processBy(EntityProcessor& processor) const override
            {
                (void)processor;
            }

        private:
            value_type value;
    };
}
#endif
