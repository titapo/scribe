#ifndef SCRIBE_COMMON_HELPERS_H_INCLUDED
#define SCRIBE_COMMON_HELPERS_H_INCLUDED

#include <type_traits>
#include <algorithm>

namespace scribe
{
    template <typename F>
    using IsFunction = typename std::enable_if<std::is_function<F>::value, F>;

    template <typename orig_type>
    struct basic_iterator_adaptor
    {
        public:
            explicit basic_iterator_adaptor(orig_type original)
                : orig(original)
            {}

            inline basic_iterator_adaptor& operator++()
            {
                orig++;
                return *this;
            }

            inline bool operator==(const basic_iterator_adaptor& rhs) const
            { return orig == rhs.orig; }
            inline bool operator!=(const basic_iterator_adaptor& rhs) const
            { return orig != rhs.orig; }

            orig_type orig;
    };

    template <typename orig_type,
             typename converter_type,
             typename = IsFunction<converter_type>
             >
    struct iterator_adaptor : basic_iterator_adaptor<orig_type>
    {
        public:
            using base = basic_iterator_adaptor<orig_type>;
            using value_type = decltype(*std::declval<orig_type>());
            using result_type = decltype(std::declval<converter_type>().operator()(std::declval<value_type>()));

            // constructor
            using base::basic_iterator_adaptor;

            iterator_adaptor(orig_type original, converter_type&& c)
              : base(original)
              , converter(std::move(c))
            {}

            iterator_adaptor(orig_type original, const converter_type& c)
              : base(original)
              , converter(c)
            {}

            inline iterator_adaptor operator+(int increment) const
            { return iterator_adaptor(base::orig + increment); }

            result_type operator*()
            {
              return converter(*(base::orig));
            }
        private:
            converter_type converter;

    };

    // inspired by ranges
    template <typename Range, typename Transformation>
    struct range_view
    {
      range_view(const Range& orig, Transformation&& tr)
        : origRange(orig)
        , transformation(std::move(tr))
      {}

      range_view(const Range& orig, const Transformation& tr)
        : origRange(orig)
        , transformation(tr)
      {}

      using const_iterator = iterator_adaptor<typename Range::const_iterator, Transformation>;

      const_iterator begin() const
      { return const_iterator(origRange.begin(), transformation); }

      const_iterator end() const
      { return const_iterator(origRange.end(), transformation); }
      
      const Range& origRange;
      Transformation transformation;
    };

    template <typename Range, typename Transformation>
    auto operator|(const Range& range, Transformation&& transformation)
    {
      return range_view<Range, Transformation>(range, std::forward<Transformation>(transformation));
    }

    // some common view adaptors
    template <size_t N>
    struct pick_nth
    {
      template <typename ValueType>
      auto operator()(const ValueType& value)
      { return std::get<N>(value); }
    };

    using pick_first = pick_nth<0>;
    using pick_second = pick_nth<1>;


    struct NonCopyable
    {
      NonCopyable() = default;
      NonCopyable(const NonCopyable&) = delete;
      NonCopyable& operator=(const NonCopyable&) = delete;
    };

    struct NonMovable
    {
      NonMovable() = default;
      NonMovable(const NonMovable&) = delete;
      NonMovable& operator=(const NonMovable&) = delete;
    };

    struct NonTransferable : NonCopyable, NonMovable
    {};

    // algorithm helpers
    // shortcut for finding something
    template <typename Range>
    auto find_in(const Range& range, const typename Range::value_type& needle)
    {
      return std::find(range.begin(), range.end(), needle);
    }

    template <typename Range>
    auto find_duplicated(const Range& range)
    {
      for (auto it = range.begin(); it != range.end(); ++it)
      {
        const auto duplicated = std::find(std::next(it), range.end(), *it);
        if (duplicated != range.end())
          return duplicated;
      }
      return range.end();
    }

    template <typename Range>
    bool contains(const Range& range, const typename Range::value_type& needle)
    {
      return find_in(range, needle) != range.end();
    }
}
#endif
