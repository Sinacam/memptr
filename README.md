# memptr

`memptr` is a fully standard compliant library used to subvert the access control system without having restricted semantics over having actual access.

# Installation

This is a header only library and it is tiny. Just `#include "memptr.hpp"`.

# Usage

````c++
class A
{
    int i;
    constexpr int foo() { return 42; }
};

auto i = GETMEM(A::i);
constexpr auto& not_so_private(A& a) { return mp::member(a, i); }
````

Which returns `a.i` as expected.

````c++
auto foo = GETMEM(A::foo);
constexpr auto not_so_private_foo(A& a) { return mp::invoke(foo, a); }
````

Which returns `42`.

````c++
constexpr auto& not_so_private_again(A& a) { return mp::member<i>(a); }
constexpr auto& not_so_private_another(A& a) { return mp::invoke(i, a); }
constexpr auto& not_so_private_extra(A& a) { return a.*memptr<i>; }
````

Also returns `a.i`, all of them being equivalent.

````c++
struct O {
    constexpr int f() { return 0; }
    constexpr int f(int) { return 1; }
};

auto f = GETMEM(int (O::*)(), O::f);

constexpr auto not_so_private(O& o) { return mp::invoke(f, o); }
````

A type may be specified to disambiguate between overloads and templates.

# Caveats

`GETMEM` may only be used in namespace scope.

Never use inline variables with `GETMEM`. `mp` uses types from unnamed namepsaces
under the hood and will cause ODR violations.

````c++
// Someheader.hpp
struct ODR { int i; };
inline auto i = GETMEM(ODR::i);     // This will be an ODR violation
````

# Disabling GETMEM

`GETMEM` can be disabled by defining `MEMPTR_NO_MACRO` before including memptr.hpp.
To set a member without the macro

````c++
struct B {
    int i;
    int f();
};

constexpr int i = 42;
template struct mp::setptr<&B::i, i>;

constexpr int f = 420;
template struct mp::setfnptr<int (B::*)(), &B::f, f>;

constexpr auto& not_so_private(B& b) { return mp::member<i>(b); }
constexpr auto not_so_private_f(B& b) {
    return mp::invoke(std::integral_constant<int, f>{}, b);
}
````

Where `i` and `f` needs to be a unique number across all declarations of `mp::setptr`
`mp::setfnptr` in a translation unit. If they aren't unique, there will be an
(arcane) compile error.

Note that passing ints as function parameters requires them to be wrapped with
 `std::integral_constant`.

# Acknowledgements
Guillaume Racicot for the [explanation](https://stackoverflow.com/questions/54909496/access-control-in-template-parameters) on _why_ such techniques are legal.  
Johannes Schaub - litb for the [original](http://bloglitb.blogspot.com/2010/07/access-to-private-members-thats-easy.html) idea.
