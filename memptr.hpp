#ifndef MEMPTR_HPP_INCLUDED
#define MEMPTR_HPP_INCLUDED

#include<type_traits>
#include<functional>

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

        template <typename Fn, Fn Val, int N>
        struct setfnptr
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

        template <int N, typename C, typename... Args>
        constexpr decltype(auto) invoke(std::integral_constant<int, N>, C&& c, Args&&... args)
        {
            return std::invoke(memptr<N>, static_cast<C&&>(c), static_cast<Args&&>(args)...);
        }
    } // namespace

} // namespace mp

#ifndef MEMPTR_NO_MACRO

#define GETDATAMEM(mem, n) \
    std::integral_constant<int, n>{};   \
    template struct mp::setptr<&mem, n>

#define GETFNMEM(T, mem, n)    \
    std::integral_constant<int, n>{};   \
    template struct mp::setfnptr<T, &mem, n>

#define MEMPTR_SELECT(_1, _2, x, ...) x
#define GETMEM(...) MEMPTR_SELECT(__VA_ARGS__, GETFNMEM, GETDATAMEM, _)(__VA_ARGS__, __COUNTER__)

#endif // MEMPTR_NO_MACRO

#endif // MEMPTR_HPP_INCLUDED
