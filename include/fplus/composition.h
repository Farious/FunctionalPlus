// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <functional>
#include <type_traits>
#include <utility>

#include "function_traits.h"

namespace fplus
{

// bind_1_of_2 : (a -> b -> c) -> a -> (b -> c)
template <typename F, typename T,
    typename FIn0 = typename utils::function_traits<F>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<F>::template arg<1>::type,
    typename FOut = typename utils::function_traits<F>::result_type>
std::function<FOut(FIn1)> bind_1_of_2(F f, T x)
{
    static_assert(utils::function_traits<F>::arity == 2, "Wrong arity.");
    static_assert(std::is_convertible<T, FIn0>::value, "Function can not take bound parameter type.");
    return [f = std::forward<F>(f), x = std::forward<T>(x)]
           (FIn1 y)
           { return f(x, y); };
}

// bind_1_of_3 : (a -> b -> c -> d) -> a -> (b -> c -> d)
template <typename F, typename X,
    typename FIn0 = typename utils::function_traits<F>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<F>::template arg<1>::type,
    typename FIn2 = typename utils::function_traits<F>::template arg<2>::type,
    typename FOut = typename utils::function_traits<F>::result_type>
std::function<FOut(FIn1, FIn2)> bind_1_of_3(F f, X x)
{
    static_assert(utils::function_traits<F>::arity == 3, "Wrong arity.");
    static_assert(std::is_convertible<X, FIn0>::value, "Function can not take bound parameter type.");
    return [f = std::forward<F>(f), x = std::forward<X>(x)]
           (FIn1 y, FIn2 z)
           { return f(x, y, z); };
}

// bind_2_of_3 : (a -> b -> c -> d) -> a -> b -> (c -> d)
template <typename F, typename X, typename Y,
    typename FIn0 = typename utils::function_traits<F>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<F>::template arg<1>::type,
    typename FIn2 = typename utils::function_traits<F>::template arg<2>::type,
    typename FOut = typename utils::function_traits<F>::result_type>
std::function<FOut(FIn2)> bind_2_of_3(F f, X x, Y y)
{
    static_assert(utils::function_traits<F>::arity == 3, "Wrong arity.");
    static_assert(std::is_convertible<X, FIn0>::value, "Function can not take first bound parameter type.");
    static_assert(std::is_convertible<Y, FIn1>::value, "Function can not take second bound parameter type.");
    return [f = std::forward<F>(f),
            x = std::forward<X>(x),
            y = std::forward<Y>(y)]
           (FIn2 z)
           { return f(x, y, z); };
}

// flip : (a -> b) -> (b -> a)
// Flips the arguments of a binary function
template <typename F,
    typename A = typename utils::function_traits<F>::template arg<0>::type,
    typename B = typename utils::function_traits<F>::template arg<1>::type,
    typename C = typename utils::function_traits<F>::result_type>
auto flip(F f)
{
    static_assert(utils::function_traits<F>::arity == 2, "Wrong arity.");
    return [f](B y, A x) { return f(x, y); };
}

// apply_to_pair : (a -> b -> c) -> (a, b) -> c
template <typename F, typename Pair,
    typename FIn0 = typename utils::function_traits<F>::template arg<0>::type,
    typename FIn1 = typename utils::function_traits<F>::template arg<1>::type,
    typename Res = typename utils::function_traits<F>::result_type>
Res apply_to_pair(F f, const Pair& pair)
{
    static_assert(utils::function_traits<F>::arity == 2, "Wrong arity.");
    static_assert(std::is_convertible<typename Pair::second_type, FIn0>::value, "Function can not take first bound parameter type.");
    static_assert(std::is_convertible<typename Pair::first_type, FIn1>::value, "Function can not take second bound parameter type.");
    return f(pair.first, pair.second);
}

// Forward composition: compose(f, g)(x) = g(f(x))
template <typename F, typename G,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename GOut = typename utils::function_traits<G>::result_type>
auto compose(F f, G g)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<G>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<FOut,GIn>::value, "Parameter types do not match");
    return [f, g](FIn x) { return g(f(x)); };
}

// Forward composition: compose(f, g, h)(x) = h(g(f(x)))
template <typename F, typename G, typename H,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename GOut = typename utils::function_traits<G>::result_type,
    typename HIn = typename utils::function_traits<H>::template arg<0>::type,
    typename HOut = typename utils::function_traits<H>::result_type>
auto compose(F f, G g, H h)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<G>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<H>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<FOut,GIn>::value, "Parameter types do not match");
    static_assert(std::is_convertible<GOut,HIn>::value, "Parameter types do not match");
    return [f, g, h](FIn x) { return h(g(f(x))); };
}

// Forward composition: compose(f, g, h, i)(x) = i(h(g(f(x))))
template <typename F, typename G, typename H, typename I,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename GOut = typename utils::function_traits<G>::result_type,
    typename HIn = typename utils::function_traits<H>::template arg<0>::type,
    typename HOut = typename utils::function_traits<H>::result_type,
    typename IIn = typename utils::function_traits<I>::template arg<0>::type,
    typename IOut = typename utils::function_traits<I>::result_type>
auto compose(F f, G g, H h, I i)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<G>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<H>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<I>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<FOut,GIn>::value, "Parameter types do not match");
    static_assert(std::is_convertible<GOut,HIn>::value, "Parameter types do not match");
    static_assert(std::is_convertible<HOut,IIn>::value, "Parameter types do not match");
    return [f, g, h, i](FIn x) { return i(h(g(f(x)))); };
}

// Forward composition: compose(f, g, h, i, j)(x) = j(i(h(g(f(x)))))
template <typename F, typename G, typename H, typename I, typename J,
    typename FIn = typename utils::function_traits<F>::template arg<0>::type,
    typename FOut = typename utils::function_traits<F>::result_type,
    typename GIn = typename utils::function_traits<G>::template arg<0>::type,
    typename GOut = typename utils::function_traits<G>::result_type,
    typename HIn = typename utils::function_traits<H>::template arg<0>::type,
    typename HOut = typename utils::function_traits<H>::result_type,
    typename IIn = typename utils::function_traits<I>::template arg<0>::type,
    typename IOut = typename utils::function_traits<I>::result_type,
    typename JIn = typename utils::function_traits<I>::template arg<0>::type,
    typename JOut = typename utils::function_traits<I>::result_type>
auto compose(F f, G g, H h, I i, J j)
{
    static_assert(utils::function_traits<F>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<G>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<H>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<I>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<J>::arity == 1, "Wrong arity.");
    static_assert(std::is_convertible<FOut,GIn>::value, "Parameter types do not match");
    static_assert(std::is_convertible<GOut,HIn>::value, "Parameter types do not match");
    static_assert(std::is_convertible<HOut,IIn>::value, "Parameter types do not match");
    static_assert(std::is_convertible<IOut,JIn>::value, "Parameter types do not match");
    return [f, g, h, i, j](FIn x) { return j(i(h(g(f(x))))); };
}

// logical_not(f) = \x -> !x
template <typename UnaryPredicate,
    typename X = typename utils::function_traits<UnaryPredicate>::template arg<0>::type>
std::function<bool(X)> logical_not(UnaryPredicate f)
{
    static_assert(utils::function_traits<UnaryPredicate>::arity == 1, "Wrong arity.");
    typedef typename utils::function_traits<UnaryPredicate>::result_type Res;
    static_assert(std::is_same<Res, bool>::value, "Must return bool.");
    return [f](X x) { return !f(x); };
}

// logical_or(f, g) = \x -> f(x) or g(x)
template <typename UnaryPredicateF, typename UnaryPredicateG,
    typename X = typename utils::function_traits<UnaryPredicateF>::template arg<0>::type,
    typename Y = typename utils::function_traits<UnaryPredicateG>::template arg<0>::type>
std::function<bool(X)> logical_or(UnaryPredicateF f, UnaryPredicateG g)
{
    static_assert(std::is_convertible<X, Y>::value, "Parameter types do not match");
    typedef typename utils::function_traits<UnaryPredicateF>::result_type FRes;
    typedef typename utils::function_traits<UnaryPredicateG>::result_type GRes;
    static_assert(std::is_same<FRes, bool>::value, "Must return bool.");
    static_assert(std::is_same<GRes, bool>::value, "Must return bool.");
    static_assert(utils::function_traits<UnaryPredicateF>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<UnaryPredicateG>::arity == 1, "Wrong arity.");
    return [f, g](X x) { return f(x) || g(x); };
}

// logical_and(f, g) = \x -> f(x) and g(x)
template <typename UnaryPredicateF, typename UnaryPredicateG,
    typename X = typename utils::function_traits<UnaryPredicateF>::template arg<0>::type,
    typename Y = typename utils::function_traits<UnaryPredicateG>::template arg<0>::type>
std::function<bool(X)> logical_and(UnaryPredicateF f, UnaryPredicateG g)
{
    static_assert(std::is_convertible<X, Y>::value, "Parameter types do not match");
    typedef typename utils::function_traits<UnaryPredicateF>::result_type FRes;
    typedef typename utils::function_traits<UnaryPredicateG>::result_type GRes;
    static_assert(std::is_same<FRes, bool>::value, "Must return bool.");
    static_assert(std::is_same<GRes, bool>::value, "Must return bool.");
    static_assert(utils::function_traits<UnaryPredicateF>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<UnaryPredicateG>::arity == 1, "Wrong arity.");
    return [f, g](X x) { return f(x) && g(x); };
}

// logical_xor(f, g) = \x -> f(x) xor g(x)
template <typename UnaryPredicateF, typename UnaryPredicateG,
    typename X = typename utils::function_traits<UnaryPredicateF>::template arg<0>::type,
    typename Y = typename utils::function_traits<UnaryPredicateG>::template arg<0>::type>
std::function<bool(X)> logical_xor(UnaryPredicateF f, UnaryPredicateG g)
{
    static_assert(std::is_convertible<X, Y>::value, "Parameter types do not match");
    typedef typename utils::function_traits<UnaryPredicateF>::result_type FRes;
    typedef typename utils::function_traits<UnaryPredicateG>::result_type GRes;
    static_assert(std::is_same<FRes, bool>::value, "Must return bool.");
    static_assert(std::is_same<GRes, bool>::value, "Must return bool.");
    static_assert(utils::function_traits<UnaryPredicateF>::arity == 1, "Wrong arity.");
    static_assert(utils::function_traits<UnaryPredicateG>::arity == 1, "Wrong arity.");
    return [f, g](X x)
    {
        auto fx = f(x);
        auto gx = g(x);
        return (fx && !gx) || (!fx && gx);
    };
}

}