// Copyright Tobias Hermann 2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "composition.h"
#include "container_common.h"
#include "transform.h"
#include "pairs.h"

#include <map>
#include <unordered_map>

namespace fplus
{

// Converts a Container of pairs (key, value) into a dictionary.
template <typename MapOut, typename ContainerIn>
MapOut pairs_to_map(const ContainerIn& pairs)
{
    return convert<MapOut>(pairs);
}

// Converts a dictionary into a Container of pairs (key, value).
template <typename MapType,
    typename MapPair = typename MapType::value_type,
    typename Key = typename std::remove_const_t<typename MapPair::first_type>,
    typename Val = typename std::remove_const_t<typename MapPair::second_type>,
    typename OutPair = std::pair<Key, Val>,
    typename ContainerOut = std::vector<OutPair>>
ContainerOut map_to_pairs(const MapType& dict)
{
    return convert<ContainerOut>(dict);
}

template <typename MapType,
    typename ContainerOut = std::vector<std::remove_const_t<typename MapType::key_type>>>
ContainerOut get_map_keys(const MapType& dict)
{
    auto pairs = map_to_pairs(dict);
    typedef typename decltype(pairs)::value_type::first_type FirstType;
    typedef typename decltype(pairs)::value_type::second_type SecondType;
    return transform(fst<FirstType, SecondType>, map_to_pairs(dict));
}

template <typename MapType,
    typename ContainerOut = std::vector<std::remove_const_t<typename MapType::mapped_type>>>
ContainerOut get_map_values(const MapType& dict)
{
    auto pairs = map_to_pairs(dict);
    typedef typename decltype(pairs)::value_type::first_type FirstType;
    typedef typename decltype(pairs)::value_type::second_type SecondType;
    return transform(snd<FirstType, SecondType>, map_to_pairs(dict));
}

// Swaps keys and Values of a dict:
// swap_keys_and_values({(1, "a"), (2, "b")}) == {("a", 1), ("b", 2)}
template <typename MapIn,
    typename MapInPair = typename MapIn::value_type,
    typename InKey = typename MapInPair::first_type,
    typename InVal = typename MapInPair::second_type,
    typename OutKey = InVal,
    typename OutVal = std::remove_const_t<InKey>,
    typename MapOut = typename SameMapTypeNewTypes<MapIn, OutKey, OutVal>::type>
MapOut swap_keys_and_values(const MapIn& dict)
{
    auto inAsPairs = map_to_pairs(dict);
    auto outAsPairs = transform(swap_pair_elems<InKey, InVal>, inAsPairs);
    return pairs_to_map<MapOut>(outAsPairs);
}

// create_map([1,2,3], ["one", "two"]) == { {1,"one"}, {2,"two"} }
template <typename ContainerIn1, typename ContainerIn2,
    typename Key = std::remove_const_t<typename ContainerIn1::value_type>,
    typename Val = std::remove_const_t<typename ContainerIn2::value_type>,
    typename MapOut = std::map<Key, Val>>
MapOut create_map(const ContainerIn1& keys, const ContainerIn2& values)
{
    auto pairs = zip(keys, values);
    return pairs_to_map<MapOut>(pairs);
}

// create_unordered_map([1,2,3], ["one", "two"]) == { {1,"one"}, {2,"two"} }
template <typename ContainerIn1, typename ContainerIn2,
    typename Key = std::remove_const_t<typename ContainerIn1::value_type>,
    typename Val = std::remove_const_t<typename ContainerIn2::value_type>,
    typename MapOut = std::unordered_map<Key, Val >>
MapOut create_unordered_map(
    const ContainerIn1& keys,
    const ContainerIn2& values)
{
    auto pairs = zip(keys, values);
    return pairs_to_map<MapOut>(pairs);
}

// Returns just the value of a key if key is present.
// Otherwise returns nothing.
template <typename MapType,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
maybe<Val> get_from_map(const MapType& map, const Key& key)
{
    auto it = map.find(key);
    if (it == std::end(map))
        return nothing<Val>();
    return just(it->second);
}

// Returns the value of a key if key is present.
// Otherwise returns the provided default.
template <typename MapType,
    typename Key = typename MapType::key_type,
    typename Val = typename MapType::mapped_type>
Val get_from_map_with_def(const MapType& map, const Val& defVal, const Key& key)
{
    return with_default(defVal, get_from_map(map, key));
}

// Checks if a map contains a key.
template <typename MapType, typename Key = typename MapType::key_type>
bool map_contains(const MapType& map, const Key& key)
{
    auto it = map.find(key);
    return it != std::end(map);
}

} // namespace fplus