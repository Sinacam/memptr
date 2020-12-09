#ifndef MEMPTR_HPP_INCLUDED
#define MEMPTR_HPP_INCLUDED

#include <functional>
#include <type_traits>
#include <utility>

namespace mp
{
    namespace detail
    {
        template <typename T>
        struct type_identity
        {
            using type = T;
        };

        template <typename T, bool C>
        using add_const_t = std::conditional_t<C, std::add_const_t<T>, T>;
        template <typename T, bool V>
        using add_volatile_t = std::conditional_t<V, std::add_volatile_t<T>, T>;
        template <typename T, bool C, bool V>
        using add_cv_t = add_const_t<add_volatile_t<T, V>, C>;

        template <typename T, typename U>
        inline auto match_cvref()
        {
            using rT = std::remove_reference_t<T>;
            constexpr bool C = std::is_const_v<rT>;
            constexpr bool V = std::is_volatile_v<rT>;

            if constexpr(std::is_rvalue_reference_v<T>)
                return type_identity<add_cv_t<U, C, V>&&>{};
            else if constexpr(std::is_reference_v<T>)
                return type_identity<add_cv_t<U, C, V>&>{};
            else
                return type_identity<add_cv_t<U, C, V>>{};
        }

        // matches the cvref qualifiers of T to U
        template <typename T, typename U>
        using match_cvref_t = typename decltype(match_cvref<T, U>())::type;

        // C style casts are weird exceptions to base access
        template <typename C, typename B, typename T,
                  std::enable_if_t<std::is_base_of_v<B, std::remove_reference_t<C>>, int> = 0>
        inline constexpr decltype(auto) member(C&& c, T(B::*p))
        {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-stack-address"
#endif // __clang__

            return (match_cvref_t<C&&, B>)c.*p;

#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__
        }

        // C style casts are weird exceptions to base access
        template <typename C, typename B, typename T, typename... Args,
                  std::enable_if_t<std::is_base_of_v<B, std::remove_reference_t<C>>, int> = 0>
        inline constexpr decltype(auto) invoke(T(B::*p), C&& c, Args&&... args)
        {
            return std::invoke(p, (match_cvref_t<C, B>)c, std::forward<Args>(args)...);
        }
    } // namespace detail

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

#define GETDATAMEM(mem, n)                                                                         \
    std::integral_constant<int, n>{};                                                              \
    template struct mp::setptr<&mem, n>

#define GETFNMEM(T, mem, n)                                                                        \
    std::integral_constant<int, n>{};                                                              \
    template struct mp::setfnptr<T, &mem, n>

#define MEMPTR_SELECT(_1, _2, x, ...) x
#define GETMEM(...) MEMPTR_SELECT(__VA_ARGS__, GETFNMEM, GETDATAMEM, _)(__VA_ARGS__, __COUNTER__)

#endif // MEMPTR_NO_MACRO

#endif // MEMPTR_HPP_INCLUDED
