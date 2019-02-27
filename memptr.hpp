#ifndef MEMPTR_HPP_INCLUDED
#define MEMPTR_HPP_INCLUDED

namespace mp
{
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-template-friend"
#endif

    template <typename T>
    struct flag
    {
        friend constexpr T adl(flag<T>);
    };

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

    template <auto Val>
    struct setptr
    {
        friend constexpr decltype(Val) adl(flag<decltype(Val)>) { return Val; }
    };

    template <typename T>
    inline constexpr auto memptr = adl(flag<T>{});
} // namespace mp

#endif // MEMPTR_HPP_INCLUDED
