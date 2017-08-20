#ifndef SCRIBE_ENTITYPROCESSOR_H_INCLUDED
#define SCRIBE_ENTITYPROCESSOR_H_INCLUDED

#include <scribe/Leaf.h>
#include <scribe/Node.h>

namespace scribe
{
    class EntityProcessor
    {
        public:
            virtual void process(const LeafBase& leaf) = 0;
            virtual void process(const Node& leaf) = 0;
    };
}

#endif
