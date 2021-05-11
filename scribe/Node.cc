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

std::unique_ptr<Entity> Node::setChild(const std::string& name, std::unique_ptr<Entity> child)
{
  try
  {
    // TODO atomicity?
    auto old = std::move(children.at(name));
    children.at(name) = std::move(child);
    return old;
  }
  catch (const std::out_of_range&)
  {
    throw NoSuchChild(makeString() << "Cannot set child! No such child in node: " << name);
  }

  return nullptr;
}

Entity& Node::getChild(const std::string& name) const
{
    try
    {
        return *children.at(name);
    }
    catch (const std::out_of_range&)
    {
        throw NoSuchChild(makeString() << "Cannot access child! No such child in node: " << name);
    }
}

Node::WeakEntry Node::getEntry(const std::string& name) const
{
    return WeakEntry(name, getChild(name));
}

std::unique_ptr<Entity> Node::removeChild(const std::string& name)
{
    auto element = children.find(name);
    if (element == children.end())
        return {nullptr};

    auto removed = std::move(element->second);
    children.erase(element);
    return removed;
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
