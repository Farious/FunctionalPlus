// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "container_common.h"
#include "function_traits.h"

#include <utility>

namespace fplus
{

// zip_with((+), [1, 2, 3], [5, 6]) == [6, 8]
template <typename ContainerIn1, typename ContainerIn2, typename F,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = typename utils::function_traits<F>::result_type,
    typename ContainerOut =
        typename same_cont_new_t<ContainerIn1, TOut>::type>
ContainerOut zip_with(const F& f,
        const ContainerIn1& xs, const ContainerIn2& ys)
{
    static_assert(utils::function_traits<F>::arity == 2, "Function must take two parameters.");
    typedef typename utils::function_traits<F>::result_type FOut;
    typedef typename utils::function_traits<F>::template arg<0>::type FIn0;
    typedef typename utils::function_traits<F>::template arg<1>::type FIn1;
    typedef typename ContainerIn1::value_type T1;
    typedef typename ContainerIn2::value_type T2;
    static_assert(std::is_convertible<T1, FIn0>::value, "Function does not take elements from first Container as first Parameter.");
    static_assert(std::is_convertible<T2, FIn1>::value, "Function does not take elements from second Container as second Parameter.");
    static_assert(std::is_convertible<FOut, TOut>::value, "Elements produced by this function can not be stored in ContainerOut.");
    static_assert(std::is_same<
        typename same_cont_new_t<ContainerIn1, void>::type,
        typename same_cont_new_t<ContainerIn2, void>::type>::value,
        "Both Containers must be of same outer type.");
    ContainerOut result;
    std::size_t resultSize = std::min(size_of_cont(xs), size_of_cont(ys));
    prepare_container(result, resultSize);
    auto itResult = get_back_inserter(result);
    auto itXs = std::begin(xs);
    auto itYs = std::begin(ys);
    for (std::size_t i = 0; i < resultSize; ++i)
    {
        *itResult = f(*(itXs++), *(itYs)++);
    }
    return result;
}

// zip([1, 2, 3], [5, 6]) == [(1, 5), (2, 6)]
template <typename ContainerIn1, typename ContainerIn2,
    typename X = typename ContainerIn1::value_type,
    typename Y = typename ContainerIn2::value_type,
    typename TOut = std::pair<X, Y>,
    typename ContainerOut =
        typename same_cont_new_t<ContainerIn1, TOut>::type>
ContainerOut zip(const ContainerIn1& xs, const ContainerIn2& ys)
{
    auto MakePair = [](const X& x, const Y& y)
            { return std::make_pair(x, y); };
    return zip_with(MakePair, xs, ys);
}

// unzip([(1, 5), (2, 6)]) == ([1, 2], [5, 6])
template <typename ContainerIn,
    typename TIn = typename ContainerIn::value_type,
    typename X = typename TIn::first_type,
    typename Y = typename TIn::second_type,
    typename ContainerOutX = typename same_cont_new_t<ContainerIn, X>::type,
    typename ContainerOutY = typename same_cont_new_t<ContainerIn, Y>::type>
std::pair<ContainerOutX, ContainerOutY> unzip(const ContainerIn& pairs)
{
    ContainerOutX firsts;
    ContainerOutY seconds;
    prepare_container(firsts, size_of_cont(pairs));
    prepare_container(seconds, size_of_cont(pairs));
    auto itFirsts = get_back_inserter(firsts);
    auto itSeconds = get_back_inserter(seconds);
    for (const auto& pair : pairs)
    {
        *itFirsts = pair.first;
        *itSeconds = pair.second;
    }
    return std::make_pair(firsts, seconds);
}

// fst((0, 1)) == 0
template <typename X, typename Y>
X fst(const std::pair<X, Y>& pair)
{
    return pair.first;
}

// snd((0, 1)) == 1
template <typename X, typename Y>
Y snd(const std::pair<X, Y>& pair)
{
    return pair.second;
}

// transform_fst(square, (4, 5)) == (16, 5)
template <typename X, typename Y, typename F,
    typename ResultFirst = typename utils::function_traits<F>::result_type>
std::pair<ResultFirst, Y> transform_fst(F f, const std::pair<X, Y>& pair)
{
    return std::make_pair(f(pair.first), pair.second);
}

// transform_snd(square, (4, 5)) == (4, 25)
template <typename X, typename Y, typename F,
    typename ResultSecond = typename utils::function_traits<F>::result_type>
std::pair<X, ResultSecond> transform_snd(F f, const std::pair<X, Y>& pair)
{
    return std::make_pair(pair.first, f(pair.second));
}

// swap_pair_elems((3,4)) == (4,3)
template <typename X, typename Y>
std::pair<Y, X> swap_pair_elems(const std::pair<X, Y>& pair)
{
    return std::make_pair(pair.second, pair.first);
}

// overlapping_pairs([0,1,2,3]) == [(0,1),(1,2),(2,3)]
template <typename ContainerOut, typename Container,
    typename T = typename Container::value_type,
    typename Pair = typename std::pair<T, T>>
ContainerOut overlapping_pairs(const Container& xs)
{
    static_assert(std::is_convertible<Pair, typename ContainerOut::value_type>::value, "ContainerOut can not store pairs of elements from ContainerIn.");
    ContainerOut result;
    if (size_of_cont(xs) < 2)
        return result;
    prepare_container(result, size_of_cont(xs) - 1);
    auto itOut = get_back_inserter(result);
    auto it1 = std::begin(xs);
    auto it2 = it1;
    std::advance(it2, 1);
    for (; it2 != std::end(xs); ++it1, ++it2)
    {
        *itOut = std::make_pair(*it1, *it2);
    }
    return result;
}

} // namespace fplus