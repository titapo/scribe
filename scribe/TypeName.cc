#include <scribe/TypeName.h>
#include <scribe/common_helpers.h>
#include <regex>

namespace scribe
{
  bool checkTypeName(const std::string& candidate)
  {
    if (candidate.empty())
      return false;

    std::smatch match;
    if (std::regex_search(candidate, match, std::regex(".*\\s.*")))
      return false;

    return true;
  }
}

using namespace scribe;

