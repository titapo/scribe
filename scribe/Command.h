#ifndef SCRIBE_COMMAND_H_INCLUDED
#define SCRIBE_COMMAND_H_INCLUDED

#include <scribe/Entity.h>

#include <functional>
#include <list>
#include <string>

namespace scribe
{
  class Command
  {
    public:

      struct Result
      {
        enum class Status
        {
          Success,
          Failure
        };

        Result(Status commandStatus, std::unique_ptr<Command>&& reverse, std::string&& errorMessage)
          : status(commandStatus)
          , reverseCommand(std::move(reverse))
          , message(std::move(errorMessage))
        {}

        Result(Result&& rhs)
          : status(rhs.status)
          , reverseCommand(std::move(rhs.reverseCommand))
          , message(std::move(rhs.message))
        {}

        const Status status;
        std::unique_ptr<Command> reverseCommand{nullptr};
        std::string message;

        static Result successful(std::unique_ptr<Command>&& reverse)
        {
          return Result{Status::Success, std::move(reverse), std::string()};
        }

        static Result failed(std::string message)
        {
          return Result{Status::Failure, {nullptr}, std::move(message)};
        }
      };

      virtual Result execute(Entity& entity) = 0; // FIXME const
      virtual ~Command() {}
  };

  class CommandWithAction : public Command
  {
    public:
      using Action = std::function<Command::Result(Entity& entity)>;
      explicit CommandWithAction(Action&& actionToExecute)
        : action(std::move(actionToExecute))
      {}

      inline Result execute(Entity& entity) override
      { return action(entity); }
    private:
      Action action;
  };

  class AddChildCommand : public Command
  {
    public:
      AddChildCommand(const std::string& childName, std::unique_ptr<Entity>&& childEntity);
      Result execute(Entity& entity) override;
    private:
      const std::string name;
      std::unique_ptr<Entity> child;
  };

  class RemoveChildCommand : public Command
  {
    public:
      explicit RemoveChildCommand(const std::string& childName);
      Result execute(Entity& entity) override;
    private:
      const std::string name;
  };

  class RenameChildCommand : public Command
  {
    public:
      RenameChildCommand(const std::string& from, const std::string& to);
      Result execute(Entity& entity) override;
    private:
      const std::string oldName;
      const std::string newName;
  };

  class UpdateChildValueCommand : public Command
  {
    public:
      UpdateChildValueCommand(const std::string& name, std::unique_ptr<Entity> newValue);
      Result execute(Entity& entity) override;
    private:
      const std::string name;
      std::unique_ptr<Entity> newValue;
  };

  class AppendCommand : public Command
  {
    public:
      AppendCommand(std::unique_ptr<Entity>&& childEntity);
      Result execute(Entity& entity) override;
    private:
      std::unique_ptr<Entity> child;
  };


  class CommandList : public Command
  {
    public:
      using Commands = std::list<std::unique_ptr<Command>>;

      explicit CommandList(Commands&& commandsToExecute);
      Result execute(Entity& entity) override;

    private:
      Commands commands;
  };
  /*
  class ReplaceChildCommand : public Command
  {
    public:
      ReplaceChildCommand(const std::string& childName, std::unique_ptr<Entity>&& toReplace);
      Result execute(Entity& entity) override;
    private:
      const std::string oldName;
      std::unique_ptr<Entity> entity;
  };
  */

  // [path] command parameters
  //
  // (!) Modifications:
  //   - may break path-s and references
  //     - so commands should be executed via the Tree, which handles mutually exclusion
  //       - so commands should pass path-s instead of references
  //       - wrap Commands Into TreeCommands
  //     - what about path/reference following
  //      - ? exporting properties? ()
  //
  // Node
  //  - AddChild (name, entity)
  //  - RemoveChild (name)
  //  - ReplaceChild (name, entity)
  //  - RenameChild (name, newName)
  //
  // Array
  //  - Insert (index, entity) //back
  //  - Append (entity) //back
  //  - Remove (index)
  //  - Replace (index, entity)
  //
  // Leaf
  //  - Set(newvalue)
  //
  //  
  // ...
  //  - Delete (entity) -> any entity // but must be passed to its parent!
  //  - Copy(entity, path) -> path must be exist (except the last element?)
  //  - Move(entity, path)
  //
}

#endif
