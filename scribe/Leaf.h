#ifndef SCRIBE_LEAF_H_INCLUDED
#define SCRIBE_LEAF_H_INCLUDED

#include <scribe/Entity.h>
#include <string>

namespace scribe
{
    class LeafBase : public Entity
    {
        public:
            void processBy(EntityProcessor& processor) override;
            void processBy(EntityProcessor& processor) const override;
            void processBy(MutableEntityProcessor& processor) override;
            virtual std::ostream& toStream(std::ostream& stream) const = 0;
    };

    template <typename T>
    class Leaf : public LeafBase
    {
        public:
            using value_type = T;
            explicit Leaf(const value_type& initial)
                : value(initial)
            {}

            explicit Leaf(value_type&& initial)
                : value(std::move(initial))
            {}

            void setValue(const value_type& newValue)
            {
                value = newValue;
            }

            void setValue(value_type&& newValue)
            {
                value = std::move(newValue);
            }

            value_type getValue() const
            {
                return value;
            }

            std::ostream& toStream(std::ostream& stream) const override
            {
                return stream << value;
            }

        private:
            value_type value;
    };

    template <>
    inline std::ostream& Leaf<bool>::toStream(std::ostream& stream) const
    {
        return stream << std::string(value ? "true" : "false");
    }
}
#endif
