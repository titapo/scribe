#include <scribe/Array.h>
#include <scribe/exception.h>

using namespace scribe;

void Array::processBy(EntityProcessor& processor)
{
    (void)processor;
    //processor.process(*this);
}
void Array::processBy(EntityProcessor& processor) const
{
    (void)processor;
    //processor.process(*this);
}

std::size_t Array::size() const
{
    return children.size();
}

Entity& Array::getChild(Index index) const
{
    try
    {
        return *children.at(index);
    }
    catch (const std::out_of_range&)
    {
        throw NoSuchChild("No such child");
    }
}

void Array::append(std::unique_ptr<Entity> child)
{
    children.push_back(std::move(child));
}

Array::iterator::iterator(Array::iterator::orig_iterator_type orig)
     : orig(orig)
{
}

Array::const_iterator::const_iterator(Array::const_iterator::orig_const_iterator_type orig)
    : orig(orig)
{
}
