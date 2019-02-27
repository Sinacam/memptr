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

You set `memptr` by explicitly instantiating `setptr`. The parameter to `memptr` is the type by which `setptr` was instantiated with.

````c++
class Point { int x, y; };

template struct mp::setptr<&Point::x>;
template struct mp::setptr<&Point::y, 1>;

constexpr auto& px(Point& p) { return p.*mp::memptr<int Point::*>; }
constexpr auto& py(Point& p) { return p.*mp::memptr<int Point::*, 1>; }
````

There is a second `int` parameter defaulted to `0` to allow for more than one value of the same type.

Everything is done at compile time. There are no uninitialized values, static initialization fiasco and overwritten values.

For the odr-savvy: everything is in an anonymous namespace, worry not.

# Acknoledgements
Guillaume Racicot for the [explanation](https://stackoverflow.com/questions/54909496/access-control-in-template-parameters) on _why_ such techniques are legal.
Johannes Schaub - litb for the [original](http://bloglitb.blogspot.com/2010/07/access-to-private-members-thats-easy.html) idea.










