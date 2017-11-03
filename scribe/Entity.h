#ifndef SCRIBE_ENTITY_H_INCLUDED
#define SCRIBE_ENTITY_H_INCLUDED

#include <memory>

namespace scribe
{
    class EntityProcessor;
    class MutableEntityProcessor;

    class Entity
    {
        public:
            virtual ~Entity() = default;

            virtual void processBy(EntityProcessor& processor) = 0;
            virtual void processBy(EntityProcessor& processor) const = 0;
            virtual void processBy(MutableEntityProcessor& processor) = 0;

            template <typename T, typename... Args>
            static std::unique_ptr<Entity> create(Args&&... args)
            {
                return std::unique_ptr<Entity>(new T(std::forward<Args...>(args)...));
            }
    };
}

#endif
