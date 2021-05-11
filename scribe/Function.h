#ifndef SCRIBE_FUNCTION_H_INCLUDED
#define SCRIBE_FUNCTION_H_INCLUDED

#include <scribe/TypeName.h>
#include <scribe/TypeRegistry.h>
#include <scribe/makeString.h>

// polyfill
namespace detail
{
  // TODO remove this
  template <typename...>
  using void_t = void;

  template <typename Callable, typename ReturnType = void, typename = void>
  struct is_invocable : std::false_type {};

  template <typename Callable>
  struct is_invocable<Callable, void, void_t<std::result_of_t<Callable>>> : std::true_type {};

  template <typename Callable, typename... Args>
  auto invoke(Callable&& f, Args&&... arguments)
  {
    return std::forward<Callable>(f)(std::forward<Args>(arguments)...);
  }
  /*

  namespace detail
  {
    template <typename Callable, typename Tuple, std::size_t...Seq>
    constexpr decltype(auto) apply_impl(Callable&& f, Tuple&& t, std::index_sequence<Seq...>)
    {
      return invoke(std::forward<Callable>(f), std::get<Seq>(std::forward<Tuple>(t))...);
    }
  }

  template <typename Callable, typename Tuple>
  constexpr decltype(auto) apply(Callable&& f, Tuple&& t)
  {
    return detail::apply_impl(std::forward<Callable>(f), std::forward<Tuple>(t),
        std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>());
  }
  */
}
namespace detail
{
  template <std::size_t N, typename Range, typename Tuple>
  void moveElementToTuple(Range& range, Tuple& t)
  {
    std::get<N>(t) = std::move(range.at(N));
  }

  template <typename Range, typename... Ts, std::size_t... Seq>
  void X(Range& r, std::tuple<Ts...>& t, std::index_sequence<Seq...>)
  {
    using expander = int[];
    (void) expander {0, ((void)moveElementToTuple<Seq>(r, t), 0)... };
  }

  template <typename Range, typename... Ts>
  void moveElementsToTuple(Range&& range, std::tuple<Ts...>& t)
  {
    X(range, t, std::make_index_sequence<sizeof...(Ts)>());
  }

}

namespace scribe
{
  class Entity;
  // callable <- lambda

  struct Unit {
    Unit() = default;
  };

  std::ostream& operator<<(std::ostream& ostr, const Unit&)
  {
    return ostr << std::string("()");
  }

  struct Signature
  {
    using Parameter = TypeName;
    using ParameterList = std::vector<Parameter>;
    using ReturnType = TypeName;
    using ArgumentList = std::vector<std::unique_ptr<Entity>>; // TODO better place?

    const std::string name;
    const ParameterList parameters;
    const ReturnType returnType;
  };

  template <typename FunctionType, typename ReturnType, typename... Arguments> // redundant
  class Function
  {
    public:
      constexpr static auto numberOfArguments()
      {
        return sizeof...(Arguments);
      }

      Function(Signature&& sign, FunctionType&& wrappedFunction)
        : signature(sign)
        , realFunction(std::move(wrappedFunction))
      {
        if (signature.parameters.size() != sizeof...(Arguments))
          throw 1;
      }

      bool isInvokableWith(const Signature::ParameterList& parameters, const TypeRegistry& registry) const
      {
        if (parameters.size() != signature.parameters.size())
          return false;

        if (!std::equal(parameters.begin(), parameters.end(), signature.parameters.begin()))
          return false;

        const auto isTypeRegistered = [&](const TypeName& type)
        {
          try
          {
            registry.getType(type);
            return true;
          }
          catch (const UnknownTypeError&)
          {
            return false;
          }
        };

        // this may be checked on creation of the function
        if (!std::all_of(parameters.begin(), parameters.end(), isTypeRegistered))
          return false;

        return isTypeRegistered(signature.returnType);
      }

      TypeName results(const Signature::ParameterList& parameters, const TypeRegistry& registry) const
      {
        if (!isInvokableWith(parameters, registry))
          throw ScribeException(makeString() << "This method is not invokable with [" << join(parameters | toUnderlying(), ", ")<< "] !");
        // TODO add reason

        return signature.returnType;
      }

      auto invoke(const TypeRegistry& registry, Signature::ArgumentList&& arguments) const
      {
        if (arguments.size() != sizeof...(Arguments))
          throw 2; // TODO

        for (std::size_t i = 0; i < signature.parameters.size(); ++i)
        {
          try
          {
            registry.validate(*(arguments.at(i)), ValidationContext(signature.parameters.at(i)));
          }
          catch (const ScribeException& ex)
          {
            throw ScribeException(makeString() << "Parameter " << i << " is invalid! " << ex.what());
          }
        }

        auto&& result = invokeImpl(std::move(arguments), std::integral_constant<bool, (numberOfArguments() > 0)>());
        registry.validate(*result, ValidationContext(signature.returnType)); // this should be checked previously
        return std::move(result);
      }

    private:
      auto invokeImpl(Signature::ArgumentList&& arguments, std::true_type) const
      {
        std::tuple<std::decay_t<Arguments>...> argTuple;
        ::detail::moveElementsToTuple(std::move(arguments), argTuple);
        return applyImpl(std::move(argTuple), std::is_same<ReturnType, void>());
      }

      auto invokeImpl(Signature::ArgumentList&& arguments, std::false_type) const
      {
        (void)arguments;
        return applyImpl(std::is_same<ReturnType, void>());
      }

      auto applyImpl(std::tuple<std::decay_t<Arguments>...>&& argTuple, std::false_type) const
      {
        return std::apply(realFunction, std::move(argTuple));
      }

      auto applyImpl(std::tuple<std::decay_t<Arguments>...>&& argTuple, std::true_type) const
      {
        std::apply(realFunction, std::move(argTuple));
        return std::unique_ptr<Entity>(new Leaf<Unit>({}));
      }

      auto applyImpl(std::false_type) const
      {
        return realFunction();
      }

      auto applyImpl(std::true_type) const
      {
        realFunction();
        return std::unique_ptr<Entity>(new Leaf<Unit>({}));
      }


      const Signature signature;
      const FunctionType realFunction;
  };

  template <typename... Arguments>
  struct FunctionFactory
  {
    template <typename FunctionType>
    auto operator()(Signature&& signature, FunctionType&& wrappedFunction)
    {
      static_assert(detail::is_invocable<FunctionType(Arguments...)>::value, "Not invocable by the given parameter list!");
      using ReturnType = std::result_of_t<FunctionType(Arguments...)>;
      return Function<FunctionType, ReturnType, Arguments...>(std::move(signature), std::move(wrappedFunction));
    }
  };
  
  // TODO wrap function which accepts a function works with primitives and transforms them to/from leaves

}
#endif
