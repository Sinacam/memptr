# `memptr`

`memptr` is a fully standard compliant library used to subvert the access control system without having restricted semantics over having actual access.

# Installation

This is a header only library and it is tiny. Just `#include "memptr.hpp"`.

# Usage

````c++
class A { int i; };

template struct mp::setptr<&A::i>;

constexpr auto& no_privacy(A& a) { return a.*mp::memptr<int A::*>; }
````

`memptr` is a `constexpr` value previously set by `setptr`.

You set `memptr` by explicitly instantiating `setptr`. The parameter to `memptr` is the type by which `setptr` was instantiated with. As with all explicit instantiations, you should only instantiate in cpp files to avoid ODR violations.

````c++
class Point { int x, y; };

template struct mp::setptr<&Point::x>;
template struct mp::setptr<&Point::y, 1>;

constexpr auto& px(Point& p) { return p.*mp::memptr<int Point::*>; }
constexpr auto& py(Point& p) { return p.*mp::memptr<int Point::*, 1>; }
````

There is a second `int` parameter defaulted to `0` to allow for more than one value of the same type.

Everything is done at compile time. There are no uninitialized values, static initialization fiasco and overwritten values.















