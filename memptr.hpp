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

        template <int N>
        struct flag
        {
            friend constexpr auto adl(flag<N>);
        };

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

        template <auto Val, int N>
        struct setptr
        {
            friend constexpr auto adl(flag<N>) { return Val; }
        };

        template <int N>
        constexpr auto memptr = adl(flag<N>{});

        template <int N, typename C>
        constexpr decltype(auto) member(C&& c, std::integral_constant<int, N> = {})
        {
            return static_cast<C&&>(c).*memptr<N>;
        }
    } // namespace

} // namespace mp

#ifndef MEMPTR_NO_MACRO
#define GETMEM_(mem, n) \
    std::integral_constant<int, n>{};    \
    template struct mp::setptr<&mem, n>

#define GETMEM(mem) \
    GETMEM_(mem, __COUNTER__)
#endif // MEMPTR_NO_MACRO

#endif // MEMPTR_HPP_INCLUDED
