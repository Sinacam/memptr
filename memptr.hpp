#ifndef MEMPTR_HPP_INCLUDED
#define MEMPTR_HPP_INCLUDED

#include<type_traits>
#include<functional>
#include<utility>

namespace mp
{
    namespace detail
    {
        template<typename T>
        struct add_ref
        {
            template<typename U>
            using as = std::remove_reference_t<U>&;
        };

        template<typename T>
        struct add_ref<T&&>
        {
            template<typename U>
            using as = std::remove_reference_t<U>&&;
        };

        template<typename T, typename U>
        using match_ref_t = typename add_ref<T>::template as<U>;

        template <typename C, typename B, typename T,
            std::enable_if_t<std::is_base_of_v<B, std::remove_reference_t<C>>, int> = 0>
        constexpr decltype(auto) member(C&& c, T (B::* p))
        {
            return (match_ref_t<C, B>)c.*p;
        }

        template <typename C, typename B, typename T, typename... Args,
            std::enable_if_t<std::is_base_of_v<B, std::remove_reference_t<C>>, int> = 0>
        constexpr decltype(auto) invoke(T (B::* p), C&& c, Args&&... args)
        {
            return std::invoke(p, (match_ref_t<C, B>)c, std::forward<Args>(args)...);
        }
    }

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
            return detail::member(std::forward<C>(c), memptr<N>);
        }

        template <int N, typename C, typename... Args>
        constexpr decltype(auto) invoke(std::integral_constant<int, N>, C&& c, Args&&... args)
        {
            return detail::invoke(memptr<N>, std::forward<C>(c), std::forward<Args>(args)...);
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
