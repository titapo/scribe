#include <tests/common.h>

#include <scribe/Array.h>
#include <scribe/Node.h>
#include <scribe/Leaf.h>
#include <scribe/Command.h>
#include <scribe/TypeNotion.h>

using namespace scribe;
using namespace Catch::Matchers;

SCENARIO("add child command", "[AddChildCommand]")
{
  GIVEN("an executed command") {
    AddChildCommand command("foo", Entity::create<Leaf<std::string>>("bar"));
    Node node;
    const auto result = command.execute(node);
    REQUIRE(result.status == Command::Result::Status::Success);
    REQUIRE(node.hasChild("foo"));
    REQUIRE(types::LeafType<std::string>().get(node.getChild("foo")).getValue() == "bar");
    WHEN("the command executed again") {
      // FIXME it has internal state which is not good. Entity should have a copy method, which copies the internal structure
      const auto secondResult = command.execute(node);
      THEN("results failure") {
        REQUIRE(secondResult.status == Command::Result::Status::Failure);
        REQUIRE(secondResult.message == "Cannot add invalid child, named 'foo'!");
      }
    }

    WHEN("reverse command invoked") {
      REQUIRE(result.reverseCommand);
      result.reverseCommand->execute(node);
      THEN("the child is removed") {
        REQUIRE(!node.hasChild("foo"));
      }
    }
  }

  GIVEN("the node already has child with that name")
  {
    AddChildCommand command("foo", Entity::create<Leaf<std::string>>("bar"));
    Node node;
    node.addChild("foo", Entity::create<Node>());
    const auto result = command.execute(node);
    REQUIRE(result.status == Command::Result::Status::Failure);
    REQUIRE(result.message == "Cannot add child! 'foo' already exists!");
  }
}

SCENARIO("append command", "[AppendCommand]")
{
  GIVEN("a command")
  {
    AppendCommand command(Entity::create<Leaf<std::string>>("alma"));

    WHEN("executed on a node")
    {
      Node node;
      /// TODO this throws an exception but it should return an error
      /// const auto result = command.execute(node);
      // THEN("it fails")
      // {
      //   REQUIRE(result.status == Command::Result::Status::Failure);
      //   REQUIRE(result.message == "asddd");
      //   REQUIRE(!node.size() == 0);
      // }
    }

    WHEN("executed on an empty array")
    {
      Array array;
      const auto result = command.execute(array);
      THEN("it succeeded")
      {
        REQUIRE(result.status == Command::Result::Status::Success);
      }

      THEN("element added")
      {
        REQUIRE(array.size() == 1);
        REQUIRE(types::LeafType<std::string>().get(array.getChild(0)).getValue() == "alma");
      }

      // TODO revert?
    }
  }
}

SCENARIO("remove child command", "[RemoveChildCommand]")
{
  GIVEN("a remove-child command")
  {
    RemoveChildCommand command("foo");
    WHEN("it is executed on an empty node")
    {
      Node node;
      const auto result = command.execute(node);
      THEN("it fail and node should not change")
      {
        REQUIRE(result.status == Command::Result::Status::Failure);
        REQUIRE(!node.hasChild("foo"));
      }
    }

    WHEN("it is executed on a node which contains the element")
    {
      Node node;
      node.addChild("foo", Entity::create<Leaf<std::string>>("bar"));
      const auto result = command.execute(node);

      THEN("it should not contain that child anymore") {
        REQUIRE(!node.hasChild("foo"));
        REQUIRE(result.status == Command::Result::Status::Success);
      }

      THEN("it can be reverted")
      {
        REQUIRE(result.reverseCommand);
        result.reverseCommand->execute(node);
        REQUIRE(node.hasChild("foo"));
        REQUIRE(types::LeafType<std::string>().get(node.getChild("foo")).getValue() == "bar");
      }
    }
  }
}

SCENARIO("Rename child", "[RenameChildCommand]")
{
  GIVEN("a rename-child command")
  {
    RenameChildCommand command("foo", "new");
    WHEN("it is executed on an empty node")
    {
      Node node;
      const auto result = command.execute(node);
      THEN("it fails")
      {
        REQUIRE(result.status == Command::Result::Status::Failure);
        REQUIRE(result.message == "Cannot rename 'foo' to 'new': no such child!");
      }
    }
    
    WHEN("it is executed on a node which contains the old name")
    {
      Node node;
      node.addChild("foo", Entity::create<Leaf<std::string>>("bar"));
      const auto result = command.execute(node);
      THEN("it is renamed")
      {
        REQUIRE(result.status == Command::Result::Status::Success);
        REQUIRE(!node.hasChild("foo"));
        REQUIRE(node.hasChild("new"));
        REQUIRE(types::LeafType<std::string>().get(node.getChild("new")).getValue() == "bar");
      }

      THEN("it can be reverted")
      {
        REQUIRE(result.reverseCommand);
        result.reverseCommand->execute(node);
        REQUIRE(!node.hasChild("new"));
        REQUIRE(node.hasChild("foo"));
        REQUIRE(types::LeafType<std::string>().get(node.getChild("foo")).getValue() == "bar");
      }
    }

    WHEN("already exists a child with the new name")
    {
      Node node;
      node.addChild("foo", Entity::create<Leaf<std::string>>("bar"));
      node.addChild("new", Entity::create<Node>());
      const auto result = command.execute(node);
      REQUIRE(result.status == Command::Result::Status::Failure);
      REQUIRE(node.hasChild("foo"));
      REQUIRE(node.hasChild("new"));
      REQUIRE(result.message == "Cannot rename 'foo' to 'new': already has 'new' child!");
    }
  }
}

SCENARIO("update child value", "[UpdateChildValueCommand]")
{
  // Currently it does not checks if the new entity has the same type or not
  GIVEN("a update-child-value command")
  {
    UpdateChildValueCommand command("name", Entity::create<Leaf<std::string>>("Joe"));
    WHEN("it is executed on an empty node")
    {
      Node node;
      const auto result = command.execute(node);
      THEN("it fails")
      {
        REQUIRE(result.status == Command::Result::Status::Failure);
        REQUIRE(result.message == "Cannot update child! 'name' does not exist!");
      }
    }

    WHEN("it is executed a node with proper child")
    {
      Node node;
      node.addChild("name", Entity::create<Leaf<std::string>>("OldJoe"));
      const auto result = command.execute(node);
      THEN("command succeded")
      {
        REQUIRE(result.status == Command::Result::Status::Success);
      }

      THEN("child value updated")
      {
        REQUIRE(node.hasChild("name"));
        REQUIRE(types::LeafType<std::string>().get(node.getChild("name")).getValue() == "Joe");
      }

      THEN("it can be reverted")
      {
        REQUIRE(result.reverseCommand);
        result.reverseCommand->execute(node);
        REQUIRE(node.hasChild("name"));
        REQUIRE(types::LeafType<std::string>().get(node.getChild("name")).getValue() == "OldJoe");
      }
    }
  }
}

SCENARIO("Command list", "[CommandList]")
{

  CommandList::Commands collect;
  collect.emplace_back(std::make_unique<AddChildCommand>("foo", Entity::create<Leaf<std::string>>("bar")));
  collect.emplace_back(std::make_unique<AddChildCommand>("integer", Entity::create<Leaf<int>>(1)));
  collect.emplace_back(std::make_unique<RenameChildCommand>("integer", "number"));
  CommandList commands(std::move(collect));

  WHEN("executed on an empty node") {
    Node node;
    const auto result = commands.execute(node);
    REQUIRE(result.status == Command::Result::Status::Success);
    REQUIRE(node.hasChild("foo"));
    REQUIRE(types::LeafType<std::string>().get(node.getChild("foo")).getValue() == "bar");
    REQUIRE(node.hasChild("number"));
    REQUIRE(types::LeafType<int>().get(node.getChild("number")).getValue() == 1);

    THEN("it can be reverted")
    {
      REQUIRE(result.reverseCommand);
      result.reverseCommand->execute(node);
      REQUIRE(node.size() == 0);
    }
  }

  WHEN("when something not succeded")
  {
    Node node;
    node.addChild("number", Entity::create<Node>());
    const auto result = commands.execute(node);
    THEN("it automatically reverted to initial state")
    {
      REQUIRE(node.size() == 1);
      REQUIRE(node.hasChild("number"));
      REQUIRE(result.status == Command::Result::Status::Failure);
      REQUIRE(result.message == "Cannot rename 'integer' to 'number': already has 'number' child!");
    }
  }
}

