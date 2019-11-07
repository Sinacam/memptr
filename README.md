# memptr

`memptr` is a fully standard compliant library used to subvert the access control system without having restricted semantics over having actual access.

# Installation

This is a header only library and it is tiny. Just `#include "memptr.hpp"`.

# Usage

````c++
class A { int i; };

auto i = GETMEM(A::i);

constexpr auto& not_so_private(A& a) { return mp::member(a, i); }
````

Which returns `a.i` as expected.

````c++
constexpr auto& not_so_private_again(A& a) { return mp::member<i>(a); }
````

Also returns `a.i`, the two being equivalent.

# Caveats

Never use inline variables with `GETMEM`. `mp` uses types from unnamed namepsaces
under the hood and will cause ODR violations.

# Disabling GETMEM

`GETMEM` can be disabled by defining `MEMPTR_NO_MACRO` before including memptr.hpp.
To set a member without the macro

````c++
struct B { int i; };

constexpr int i = 42;
template struct mp::setptr<&B::i, i>;

constexpr auto& not_so_private(B& b) { return mp::member<i>(b); }
constexpr auto& not_so_private_again(B& b) {
    return mp::member(b, std::integral_constant<int, i>{});
}
````

Where `i` needs to be a unique number across all declarations of `mp::setptr`
in a translation unit. If `i` isn't unique, there will be an (arcane) compile error.

# Acknowledgements
Guillaume Racicot for the [explanation](https://stackoverflow.com/questions/54909496/access-control-in-template-parameters) on _why_ such techniques are legal.  
Johannes Schaub - litb for the [original](http://bloglitb.blogspot.com/2010/07/access-to-private-members-thats-easy.html) idea.
