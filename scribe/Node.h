#ifndef SCRIBE_NODE_H_INCLUDED
#define SCRIBE_NODE_H_INCLUDED

#include <cstddef>
#include <string>
#include <memory>
#include <unordered_map>

#include <scribe/Entity.h>
#include <scribe/exception.h>


class EntityProcessor;

namespace scribe
{

    class NoSuchChild : public ScribeException
    {
        public:
            using ScribeException::ScribeException;
    };


    class Node : public Entity
    {
        public:
            std::size_t size() const;

            void addChild(const std::string& name, std::unique_ptr<Entity> child);

            Entity& getChild(const std::string& name);

            void processBy(EntityProcessor& processor) override
            {
                (void)processor;
            }
            void processBy(EntityProcessor& processor) const override
            {
                (void)processor;
            }
        private:
            std::unordered_map<std::string, std::unique_ptr<Entity>> children;
    };
}
#endif

