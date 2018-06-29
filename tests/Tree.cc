#include <tests/catch.hpp>
#include <tests/tree_helpers.h>

#include <scribe/TypeNotion.h>
#include <scribe/Tree.h>

using namespace scribe;

#include <scribe/EntityFormatter.h>
TEST_CASE("tree")
{
  Tree tree(createTreeNode());
  REQUIRE_NOTHROW(types::NodeType().get(tree.getRoot()).getChild("colors"));
  REQUIRE(&tree.getCurrent() == &tree.getRoot());
}

TEST_CASE("tree and path")
{
  Path::Elements elements;
  elements.push_back(std::make_unique<Child>("colors"));
  elements.push_back(std::make_unique<Child>("green"));
  Tree tree(createTreeNode(), Path(std::move(elements)));
  REQUIRE(types::LeafType<std::string>().get(tree.getCurrent()).getValue() == "#00ff00");
}
