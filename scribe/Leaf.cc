#include <scribe/Leaf.h>
#include <scribe/EntityProcessor.h>

using namespace scribe;

void LeafBase::processBy(EntityProcessor& processor)
{
    processor.process(*this);
}

void LeafBase::processBy(EntityProcessor& processor) const
{
    processor.process(*this);
}

void LeafBase::processBy(MutableEntityProcessor& processor)
{
    processor.process(*this);
}
