#ifndef MEMPTR_HPP_INCLUDED
#define MEMPTR_HPP_INCLUDED

namespace mp
{
    namespace
    {
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-template-friend"
#endif

        template <typename T, int N>
        struct flag
        {
            friend constexpr T adl(flag<T, N>);
        };

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

        template <auto Val, int N = 0>
        struct setptr
        {
            friend constexpr decltype(Val) adl(flag<decltype(Val), N>) { return Val; }
        };

        template <typename T, int N = 0>
        constexpr auto memptr = adl(flag<T, N>{});
    } // namespace

} // namespace mp

#endif // MEMPTR_HPP_INCLUDED
