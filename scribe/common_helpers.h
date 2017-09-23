#ifndef SCRIBE_COMMON_HELPERS_H_INCLUDED
#define SCRIBE_COMMON_HELPERS_H_INCLUDED

namespace scribe
{

template <typename orig_type>
    class iterator_adaptor_to
    {
        public:
            iterator_adaptor_to(orig_type orig)
                : orig(orig)
            {}

            inline iterator_adaptor_to& operator++()
            {
                orig++;
                return *this;
            }

            inline bool operator==(const iterator_adaptor_to& rhs) const
            { return orig == rhs.orig; }
            inline bool operator!=(const iterator_adaptor_to& rhs) const
            { return orig != rhs.orig; }

            orig_type orig;
    };
}
#endif