#include <scribe/Node.h>
#include <scribe/EntityProcessor.h>

using namespace scribe;

std::size_t Node::size() const
{
    return children.size();
}

void Node::addChild(OwnerEntry&& entry)
{
    children.emplace(std::move(entry));
}

void Node::addChild(const std::string& name, std::unique_ptr<Entity> child)
{
    children.emplace(name, std::move(child));
}

Entity& Node::getChild(const std::string& name) const
{
    try
    {
        return *children.at(name);
    }
    catch (const std::out_of_range&)
    {
        throw NoSuchChild("No such child");
    }
}

Node::WeakEntry Node::getEntry(const std::string& name) const
{
    return WeakEntry(name, getChild(name));
}

void Node::processBy(EntityProcessor& processor)
{
    processor.process(*this);
}

void Node::processBy(EntityProcessor& processor) const
{
    processor.process(*this);
}

Node::iterator::iterator(Node::iterator::orig_iterator_type orig)
    : orig(orig)
{
}

Node::const_iterator::const_iterator(Node::const_iterator::orig_const_iterator_type orig)
    : orig(orig)
{
}
