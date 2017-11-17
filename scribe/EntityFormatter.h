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
                using IndentationLevel = unsigned;
                DisplayContext(IndentationLevel indentation = 0)
                    : indentationLevel(indentation)
                {}

                DisplayContext deeper() const
                { return DisplayContext(indentationLevel + 1); }

                std::string indentation() const;

                IndentationLevel indentationLevel{0};
            };

            explicit EntityFormatter(std::ostream& str);
            EntityFormatter(std::ostream& str, DisplayContext context);

            std::ostream& display(const Entity& entity);
            std::ostream& display(const Node::WeakEntry& entry);

            void process(const LeafBase& leaf) override;
            void process(const Node& node) override;
            void process(const Array& array) override;

        private:
            std::ostream& stream;
            const DisplayContext context;
    };

    // Helper struct
    // std::cout << printEntity(myEntityReference) << std::endl;
    // Later it should be used with other Formatters. This is why it is not a static method of EntityFormatter.
    struct printEntity
    {
        printEntity(const Entity& printedEntity)
            : entity(printedEntity)
        {}

        const Entity& entity;
    };

    inline std::ostream& operator<<(std::ostream& ostr, const printEntity& printer)
    {
        return EntityFormatter(ostr).display(printer.entity);
    }

}

#endif
