// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "function_traits.h"

#include <cassert>
#include <functional>
#include <memory>

namespace fplus
{

// Can hold a value of type T or nothing.
template <typename T>
class maybe
{
public:
    maybe() {}
    maybe(const maybe<T>& other) : ptr_(other.get() ? std::make_unique<T>(*other.get()) : ptr_t()) {}
    explicit maybe(const T& val) : ptr_(std::make_unique<T>(val)) {}
    bool is_just() const { return static_cast<bool>(get()); }
    const T& unsafe_get_just() const { assert(is_just()); return *get(); }
    typedef T type;
private:
    typedef std::unique_ptr<T> ptr_t;
    const ptr_t& get() const { return ptr_; }
    ptr_t ptr_;
};

// Is not nothing?
template <typename T>
bool is_just(const maybe<T>& maybe)
{
    return maybe.is_just();
}

// Has no value?
template <typename T>
bool is_nothing(const maybe<T>& maybe)
{
    return !is_just(maybe);
}

// Crashes if maybe is nothing!
template <typename T>
T unsafe_get_just(const maybe<T>& maybe)
{
    return maybe.unsafe_get_just();
}

// Get the value from a maybe or the default in case it is nothing.
template <typename T>
T with_default(const T& defaultValue, const maybe<T>& maybe)
{
    if (is_just(maybe))
        return unsafe_get_just(maybe);
    return defaultValue;
}

// Wrap a value in a Maybe as a Just.
template <typename T>
maybe<T> just(const T& val)
{
    return maybe<T>(val);
}

// Construct a nothing of a certain Maybe type.
template <typename T>
maybe<T> nothing()
{
    return maybe<T>();
}

// True if just values are the same or if both are nothing.
template <typename T>
bool operator == (const maybe<T>& x, const maybe<T>& y)
{
    if (is_just(x) && is_just(y))
        return unsafe_get_just(x) == unsafe_get_just(y);
    return is_just(x) == is_just(y);
}

// False if just values are the same or if both are nothing.
template <typename T>
bool operator != (const maybe<T>& x, const maybe<T>& y)
{
    return !(x == y);
}

// Lifts a function into the maybe functor.
// A function that for example was able to convert and int into a string,
// now can convert a Maybe<int> into a Maybe<string>.
// A nothings stays a nothing, regardless of the conversion.
template <typename F,
    typename A = typename std::remove_const_t<typename std::remove_reference_t<typename utils::function_traits<F>::template arg<0>::type>>,
    typename B = typename std::remove_const_t<typename std::remove_reference_t<typename utils::function_traits<F>::result_type>>>
std::function<maybe<B>(const maybe<A>&)> lift(F f)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    return [f](const maybe<A>& m)
    {
        if (is_just(m))
            return maybe<B>(f(unsafe_get_just(m)));
        return nothing<B>();
    };
}

// Monadic bind.
// Composes two functions taking a value and returning Maybe.
// If the first function returns a just, the value from the just
// is extracted and shoved into the second function.
// If the first functions returns a nothing, it stays a nothing.
template <typename F, typename G,
    typename FIn = typename std::remove_const_t<typename std::remove_reference_t<typename utils::function_traits<F>::template arg<0>::type>>,
    typename FOut = typename std::remove_const_t<typename std::remove_reference_t<typename utils::function_traits<F>::result_type>>,
    typename GIn = typename std::remove_const_t<typename std::remove_reference_t<typename utils::function_traits<G>::template arg<0>::type>>,
    typename GOut = typename std::remove_const_t<typename std::remove_reference_t<typename utils::function_traits<G>::result_type>>,
    typename C = typename GOut::type>
std::function<maybe<C>(const FIn&)> and_then(F f, G g) {
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<G>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<typename FOut::type,GIn>::value, "Function parameter types do not match");
    return [f, g](const FIn& x)
    {
        auto maybeB = f(x);
        if (is_just(maybeB))
            return g(unsafe_get_just(maybeB));
        return nothing<C>();
    };
}

} // namespace fplus