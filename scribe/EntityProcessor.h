#ifndef SCRIBE_ENTITYPROCESSOR_H_INCLUDED
#define SCRIBE_ENTITYPROCESSOR_H_INCLUDED


namespace scribe
{

    class LeafBase;
    class Node;
    class Array;

    class EntityProcessor
    {
        public:
            virtual void process(const LeafBase& leaf) = 0;
            virtual void process(const Node& node) = 0;
            virtual void process(const Array& array) = 0;
    };

    class MutableEntityProcessor
    {
        public:
            virtual void process(LeafBase& leaf) = 0;
            virtual void process(Node& node) = 0;
            virtual void process(Array& array) = 0;
    };
}

#endif
