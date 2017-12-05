#include <scribe/PathFormat.h>
#include <scribe/exception.h>
#include <regex>

using namespace scribe;

namespace
{
  template <typename To, typename From>
  struct ArgumentConverter {
    To operator()(const From& t)
    { return std::stoi(t);}
  };

  template <typename T>
  struct ArgumentConverter<T, T> {
    const T& operator()(const T& t)
    { return t; }
  };

  template <typename To, typename From>
  auto convertArgument(const From& from)
  {
    return ArgumentConverter<To, From>()(from);
  }

  template <typename Rule> // TODO specify
  PathElement::Ptr createPathElement(const std::string& match)
  {
    return PathElement::Ptr(new typename Rule::element_type(convertArgument<typename Rule::arg_type>(match)));
  }

  template <typename Rule>
  std::smatch matchRule(const Rule& rule, std::string::const_iterator& first, std::string::const_iterator& last)
  {
    std::smatch match;
    std::regex_match(first, last, match, rule.pattern);
    return match;
  }
}

PathElement::Ptr PathFormat::parseElement(const std::string& instr)
{
  auto first = instr.cbegin();
  auto last = instr.cend();
  return parseElement(first, last);
}

BasicPathFormat::BasicPathFormat()
  : rules(
      {SubstitutionType::Word, "\\.<>"},
      {SubstitutionType::Integer, "\\[<>\\]"})
{
}

PathElement::Ptr BasicPathFormat::parseElement(std::string::const_iterator& first, std::string::const_iterator& last)
{
  {
    const auto rule = std::get<0>(rules);
    const auto match = matchRule(rule, first, last);
    if (!match.empty())
    {
      std::advance(first, match.position(1) + match.length(1));
      return createPathElement<decltype(rule)>(match[2]);
    }
  }
  {
    const auto rule = std::get<1>(rules);
    const auto match = matchRule(rule, first, last);
    if (!match.empty())
    {
      std::advance(first, match.position(1) + match.length(1));
      return createPathElement<decltype(rule)>(match[2]);
    }
  }

  throw ScribeException("Cannot parse path element");
}

Path BasicPathFormat::parse(const std::string& instr)
{
  Path path;

  auto first = instr.cbegin();
  auto last = instr.cend();

  while (first != last)
  {
    path.extend(BasicPathFormat::parseElement(first, last));
  }

  return path;
}
