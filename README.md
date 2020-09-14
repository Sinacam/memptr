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
mp::member(a, i);
mp::member<i>(a);
mp::invoke(i, a);
````

Are all equivalent, meaning `a.i`.

````c++
struct O {
    constexpr int f() { return 0; }
    constexpr int f(int) { return 1; }
};

auto f = GETMEM(int (O::*)(), O::f);

constexpr auto not_so_private(O& o) { return mp::invoke(f, o); }
````

A type may be specified to disambiguate between overloads and templates.


````c++
A a;
const A ca;
mp::member(a, i);       // int&
mp::member(ca, i);      // const int&
mp::member(A{}, i);     // int&&
````

Respects qualifiers.

````c++
class B : A {};
mp::member(B{}, i);
````

Works with private base classes.

# Caveats

`GETMEM` may only be used in namespace scope.

Never use inline variables with `GETMEM`. `mp` uses types from unnamed namepsaces
under the hood and will cause ODR violations.

Doesn't work with ambiguous bases, aka diamond inheritance.

````c++
// Someheader.hpp
struct ODR { int i; };
inline auto i = GETMEM(ODR::i);     // This will be an ODR violation
````

# Disabling GETMEM

`GETMEM` can be disabled by defining `MEMPTR_NO_MACRO` before including memptr.hpp.
To set a member without the macro

````c++
#define MEMPTR_NO_MACRO
#include<memptr.hpp>

struct B {
    int i;
    int g();
};

constexpr int i = 42;
template struct mp::setptr<&B::i, i>;

constexpr int g = 420;
template struct mp::setfnptr<int (B::*)(), &B::g, g>;

constexpr auto& not_so_private(B& b) { return mp::member<i>(b); }
constexpr auto not_so_private_g(B& b) {
    return mp::invoke(std::integral_constant<int, g>{}, b);
}
````

Where `i` and `g` needs to be a unique number across all declarations of `mp::setptr`
`mp::setfnptr` in a translation unit. If they aren't unique, there will be an
(arcane) compile error.

Note that passing ints as function parameters requires them to be wrapped with
 `std::integral_constant`.

# Acknowledgements
Guillaume Racicot for the [explanation](https://stackoverflow.com/questions/54909496/access-control-in-template-parameters) on _why_ such techniques are legal.  
Johannes Schaub - litb for the [original](http://bloglitb.blogspot.com/2010/07/access-to-private-members-thats-easy.html) idea.
