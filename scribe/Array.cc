#include <scribe/Array.h>
#include <scribe/exception.h>
#include <scribe/EntityProcessor.h>

using namespace scribe;

void Array::processBy(EntityProcessor& processor)
{
    processor.process(*this);
}
void Array::processBy(EntityProcessor& processor) const
{
    processor.process(*this);
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
