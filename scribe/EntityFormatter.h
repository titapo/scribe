#ifndef SCRIBE_ENTITYFORMATTER_H_INCLUDED
#define SCRIBE_ENTITYFORMATTER_H_INCLUDED

#include <scribe/EntityProcessor.h>
#include <scribe/Leaf.h>
#include <scribe/Node.h>
#include <scribe/Array.h>
#include <iostream>

namespace scribe
{
    template <typename T>
    std::ostream& leafToStream(std::ostream& str, const Leaf<T>& leaf)
    {
        return str << leaf.getValue();
    }

    class EntityFormatter : public EntityProcessor
    {
        public:
            struct DisplayContext
            {
                DisplayContext(unsigned indentationLevel = 0)
                    : indentationLevel(indentationLevel)
                {}

                DisplayContext deeper() const
                { return DisplayContext(indentationLevel + 1); }

                std::string indentation() const;

                unsigned indentationLevel{0};
            };
            explicit EntityFormatter(std::ostream& str);
            EntityFormatter(std::ostream& str, DisplayContext context);

            void display(const Node::WeakEntry& entry, DisplayContext context = {});

            void process(const LeafBase& leaf) override;
            void process(const Node& node) override;
            void process(const Array& array) override;

        private:
            std::ostream& addIndentation();

            std::ostream& stream;
            DisplayContext context;
    };

}

#endif