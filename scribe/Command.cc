#include <scribe/Command.h>
#include <scribe/TypeNotion.h>
#include <scribe/Array.h>
#include <scribe/Node.h>
#include <scribe/makeString.h>

using namespace scribe;

AddChildCommand::AddChildCommand(const std::string& childName, std::unique_ptr<Entity>&& childEntity)
  : name(childName)
  , child(std::move(childEntity))
{
}

Command::Result AddChildCommand::execute(Entity& entity)
{
  if (!child)
    return Result::failed(makeString() << "Cannot add invalid child, named '" << name << "'!");

  Node& node = types::NodeType().get(entity);
  if (node.hasChild(name))
    return Result::failed(makeString() << "Cannot add child! '" << name << "' already exists!");

  node.addChild(name, std::move(child));
  return Result::successful(std::make_unique<RemoveChildCommand>(name));
}

RemoveChildCommand::RemoveChildCommand(const std::string& childName)
  : name(childName)
{
}

Command::Result RemoveChildCommand::execute(Entity& entity)
{
  Node& node = types::NodeType().get(entity);
  if (!node.hasChild(name))
    return Result::failed(makeString() << "Cannot remove nonexisting child: '" << name << "'!");
  auto&& removed = node.removeChild(name);
  return Result::successful(std::make_unique<AddChildCommand>(name, std::move(removed)));
}

RenameChildCommand::RenameChildCommand(const std::string& from, const std::string& to)
  : oldName(from)
  , newName(to)
{}

Command::Result RenameChildCommand::execute(Entity& entity)
{
  Node& node = types::NodeType().get(entity);
  if (!node.hasChild(oldName))
    return Result::failed(makeString() << "Cannot rename '" << oldName << "' to '" << newName << "': no such child!");

  if (node.hasChild(newName))
    return Result::failed(makeString() << "Cannot rename '" << oldName << "' to '" << newName << "': already has '" << newName << "' child!");
    //return Result::failed(makeString() << "Cannot rename '" << oldName << "' to '" << newName << "': no such child!");

  auto&& removed = node.removeChild(oldName);
  node.addChild(newName, std::move(removed));
  return Result::successful(std::make_unique<RenameChildCommand>(newName, oldName));
}

UpdateChildValueCommand::UpdateChildValueCommand(const std::string& p_name, std::unique_ptr<Entity> p_newValue)
  : name(p_name)
  , newValue(std::move(p_newValue))
{}

Command::Result UpdateChildValueCommand::execute(Entity& entity)
{
  if (!newValue)
    return Result::failed(makeString() << "Cannot add invalid child, named '" << name << "'!");

  Node& node = types::NodeType().get(entity);
  if (!node.hasChild(name))
    return Result::failed(makeString() << "Cannot update child! '" << name << "' does not exist!");

  auto oldValue = node.setChild(name, std::move(newValue));
  return Result::successful(std::make_unique<UpdateChildValueCommand>(name, std::move(oldValue)));
}

AppendCommand::AppendCommand(std::unique_ptr<Entity>&& entity)
  : child(std::move(entity))
{
}

Command::Result AppendCommand::execute(Entity& entity)
{
  if (!child)
    return Result::failed(makeString() << "Cannot append invalid child!");

  // TODO this should be returned as a failure, not as an exception
  try
  {
    Array& array = types::ArrayType().get(entity);

    array.append(std::move(child));
    // TODO implement removeElement for Array
    return Result::successful(nullptr);
  }
  catch (const ScribeException& ex)
  {
    return Result::failed(makeString() << "Cannot append element! " << ex.what());
  }
}

CommandList::CommandList(Commands&& commandsToExecute)
  : commands(std::move(commandsToExecute))
{
  for (auto&& command : commandsToExecute)
    commands.emplace_back(std::move(command));
}

namespace {
  class CommandListReverter
  {
    public:
      CommandListReverter()
      {}

      void addCommand(std::unique_ptr<Command>&& command)
      {
        commands.emplace_front(std::move(command));
      }

      std::unique_ptr<Command> toCommandList() &&
      {
        return std::make_unique<CommandList>(std::move(commands));
      }

      Command::Result revert(Entity& entity, const std::string& message)
      {
        auto revertResult = CommandList(std::move(commands)).execute(entity);
        if (revertResult.status == Command::Result::Status::Failure)
        {
          return Command::Result::failed(makeString() << "Critical error! Command reverting failed: "
              << revertResult.message << " (Revert reason: " << message << ")");
        }

        return Command::Result::failed(message);
      }

    private:
      CommandList::Commands commands;
  };
}
Command::Result CommandList::execute(Entity& entity)
{
  CommandListReverter reverter;
  for (const auto& command : commands)
  {
    auto result = command->execute(entity);
    if (result.status == Result::Status::Failure)
      return reverter.revert(entity, result.message);

    if (result.reverseCommand == nullptr)
      return reverter.revert(entity, "A command would not be reverted, commandlist aborted");

    reverter.addCommand(std::move(result.reverseCommand));
  }

  return Result::successful(std::move(reverter).toCommandList());
}
