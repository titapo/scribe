#include <scribe/Node.h>
#include <scribe/EntityProcessor.h>
#include <scribe/makeString.h>

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

bool Node::hasChild(const std::string& name) const
{
  return children.find(name) != children.end();
}

Entity& Node::getChild(const std::string& name) const
{
    try
    {
        return *children.at(name);
    }
    catch (const std::out_of_range&)
    {
        throw NoSuchChild(makeString() << "No such child in node: " << name);
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

void Node::processBy(MutableEntityProcessor& processor)
{
  processor.process(*this);
}
