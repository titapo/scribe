#ifndef TESTS_TREE_HELPERS_H_INCLUDED
#define TESTS_TREE_HELPERS_H_INCLUDED

#include <scribe/Node.h>
#include <scribe/Array.h>
#include <scribe/Leaf.h>

using namespace scribe;

inline std::unique_ptr<Entity> createTreeNode()
{
    auto root = Entity::create<Node>();
    auto& rootRef = dynamic_cast<Node&>(*root);
    rootRef.addChild("colors", Entity::create<Node>());
    auto& colors = dynamic_cast<Node&>(rootRef.getChild("colors"));
    colors.addChild("red", Entity::create<Leaf<std::string>>("#ff0000"));
    colors.addChild("green", Entity::create<Leaf<std::string>>("#00ff00"));
    colors.addChild("blue", Entity::create<Leaf<std::string>>("#0000ff"));

    rootRef.addChild("some numbers", Entity::create<Node>());
    auto& nums = dynamic_cast<Node&>(rootRef.getChild("some numbers"));
    nums.addChild("one", Entity::create<Leaf<int>>(1));
    nums.addChild("two", Entity::create<Leaf<int>>(2));
    nums.addChild("three", Entity::create<Leaf<int>>(3));

    rootRef.addChild("some names", Entity::create<Array>());
    auto& names = dynamic_cast<Array&>(rootRef.getChild("some names"));
    names.append(Entity::create<Leaf<std::string>>("Joe"));
    names.append(Entity::create<Leaf<std::string>>("Greg"));
    names.append(Entity::create<Leaf<unsigned>>(1));

    return root;
}

#endif
