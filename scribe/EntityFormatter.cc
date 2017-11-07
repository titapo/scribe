#include <scribe/EntityFormatter.h>
#include <sstream>

using namespace scribe;

EntityFormatter::EntityFormatter(std::ostream& str)
    : stream(str)
{}

EntityFormatter::EntityFormatter(std::ostream& str, DisplayContext context)
    : stream(str)
    , context(std::move(context))
{}

std::ostream& EntityFormatter::display(const Entity& entity)
{
  entity.processBy(*this);
  return stream;
}

std::ostream& EntityFormatter::display(const Node::WeakEntry& entry)
{
    stream << entry.first << ": ";
    return display(entry.second);
}

void EntityFormatter::process(const LeafBase& leaf)
{
    leaf.toStream(stream);
}

void EntityFormatter::process(const Node& node)
{
    stream << "{";
    const auto nested = context.deeper();
    EntityFormatter formatter(stream, nested);
    for (const auto& childEntry : node)
    {
        stream << std::endl << nested.indentation();
        formatter.display(childEntry);
    }
    stream << std::endl << context.indentation() << "}";
}

void EntityFormatter::process(const Array& array)
{
    stream << "[";
    const auto nested = context.deeper();
    EntityFormatter formatter(stream, nested);
    for (const auto& child : array)
    {
        stream << std::endl << nested.indentation();
        child.processBy(formatter);
    }
    stream << std::endl << context.indentation() << "]";
}

std::string EntityFormatter::DisplayContext::indentation() const
{
    std::stringstream stream;
    for (unsigned level = 0; level < indentationLevel; ++level)
        stream << "  ";

    return stream.str();
}
