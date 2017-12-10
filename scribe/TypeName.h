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
}

#endif
