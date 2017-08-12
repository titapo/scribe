#ifndef SCRIBE_ENTITY_H_INCLUDED
#define SCRIBE_ENTITY_H_INCLUDED

namespace scribe
{
    class EntityProcessor;
    class Entity
    {
        public:
            virtual void processBy(EntityProcessor& processor) = 0;
            virtual void processBy(EntityProcessor& processor) const = 0;
    };
}

#endif
