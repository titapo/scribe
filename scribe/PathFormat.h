#ifndef SCRIBE_PATHFORMAT_H_INCLUDED
#define SCRIBE_PATHFORMAT_H_INCLUDED

#include <regex>
#include <scribe/Path.h>
#include <scribe/exception.h>

namespace scribe
{
  class PathFormat
  {
    public:
      virtual PathElement::Ptr parseElement(std::string::const_iterator& first, std::string::const_iterator& last) = 0;
      virtual Path parse(const std::string& instr) = 0;
      virtual ~PathFormat() {}

      PathElement::Ptr parseElement(const std::string& instr);

  };

  enum class SubstitutionType
  {
    Word,
    Integer
  };

  template <typename PathElementType, typename Arg>
  struct FormatRule
  {
    using element_type = PathElementType;
    using arg_type = Arg;

    FormatRule(SubstitutionType argtype, const std::string& rawPattern)
      : type(argtype)
      , pattern(makeFormatRulePattern(rawPattern, type))
    {}


    static std::string getSubstitutionRegex(SubstitutionType type)
    {
      switch (type)
      {
        case SubstitutionType::Word: return "([\\w]+)";
        case SubstitutionType::Integer: return "([\\d]+)";
      }
      throw ScribeException("Critical error! Unknown format type!");
    }

    static std::regex makeFormatRulePattern(const std::string& pattern, SubstitutionType type)
    {
      std::string realPattern = std::regex_replace(pattern, std::regex("<>"), getSubstitutionRegex(type));
      return std::regex(std::string("(") + realPattern + ").*");
    }

    const SubstitutionType type;
    const std::regex pattern;
  };

  class BasicPathFormat : public PathFormat
  {
    public:
      BasicPathFormat();
      using PathFormat::parseElement;
      PathElement::Ptr parseElement(std::string::const_iterator& first, std::string::const_iterator& last) override;
      Path parse(const std::string& instr) override;

    private:
      std::tuple<
        FormatRule<Child, std::string>,
        FormatRule<Index, unsigned>> rules;
  };
}

#endif
