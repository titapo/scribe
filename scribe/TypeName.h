#ifndef SCRIBE_TYPENAME_H_INCLUDED
#define SCRIBE_TYPENAME_H_INCLUDED

#include <string>

namespace scribe
{
  class TypeName
  {
    public:
      explicit TypeName(std::string tname)
        : name(std::move(tname))
      {}

      TypeName(const TypeName& other)
        : name(other.get())
      {}

      TypeName(TypeName&& other)
        : name(std::move(other.get()))
      {}

      std::string get() const
      {
        return name;
      }

      explicit operator std::string() const
      {
        return name;
      }

      bool operator==(const TypeName& other) const
      { return get() == other.get(); }

      bool operator!=(const TypeName& other) const
      { return get() != other.get(); }

    private:
        const std::string name;
  };

  bool checkTypeName(const std::string& candidate);
}

namespace std
{
  template<> struct hash<scribe::TypeName>
  {
    using argument_type = scribe::TypeName;
    using result_type = std::size_t;
    result_type operator()(argument_type const& s) const noexcept
    {
      return std::hash<std::string>{}(s.get());
    }
  };
}

#endif
