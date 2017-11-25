#ifndef SCRIBE_COMMON_HELPERS_H_INCLUDED
#define SCRIBE_COMMON_HELPERS_H_INCLUDED

#include <type_traits>
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
            using basic_iterator_adaptor<orig_type>::basic_iterator_adaptor;
            using result_type = decltype(std::declval<converter_type>().operator()(std::declval<orig_type>()));

            result_type operator*()
            {
              return converter(basic_iterator_adaptor<orig_type>::orig);
            }
        private:
            converter_type converter;

    };

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
}
#endif
