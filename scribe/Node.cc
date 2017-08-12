#include <scribe/Node.h>

using namespace scribe;

std::size_t Node::size() const
{
    return children.size();
}

void Node::addChild(const std::string& name, std::unique_ptr<Entity> child)
{
    children.emplace(name, std::move(child));
}

Entity& Node::getChild(const std::string& name)
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
