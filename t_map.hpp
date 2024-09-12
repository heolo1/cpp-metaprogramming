#pragma once

#include <cstddef>
#include <utility>
#include <type_traits>
#include "t_util.hpp"
#include "t_list.hpp"

namespace { // _first_type and _second_type

template <IsSpecializationOf<std::pair> Pair>
using _first_type = Pair::first_type;

template <IsSpecializationOf<std::pair> Pair>
using _second_type = Pair::second_type;

}

template <IsSpecializationOf<std::pair>... Pairs>
requires (t_list<Pairs...>::template map_t<_first_type>::is_completely_heterogeneous)
struct t_map;

namespace {

template <IsSpecializationOf<std::pair>... Pairs>
using _to_unique_pairs = t_list<Pairs...>::template filter_with_rest<
	[]<IsSpecializationOf<std::pair> First, IsSpecializationOf<std::pair>... Rest>
		{ return !t_list<Rest...>::template map_t<_first_type>::template contains<typename First::first_type>; }
	>;

}

template <IsSpecializationOf<std::pair>... Pairs>
requires (t_list<Pairs...>::template map_t<_first_type>::is_completely_heterogeneous)
struct t_map {
	using pairs = t_list<Pairs...>;
	using keys = pairs::template map_t<_first_type>;
	using values = pairs::template map_t<_second_type>;

	constexpr static inline const std::size_t size = sizeof...(Pairs);

	template <typename T>
	constexpr static inline const bool contains_key = keys::template contains<T>;

	template <std::size_t Idx>
	requires (Idx < size)
	using get_key = keys::template get<Idx>;

	template <typename T>
	requires (contains_key<T>)
	using get_value = values::template get<keys::template find<T>>;

	template <typename T, typename Default = std::nullptr_t>
	using get_value_default = values::template push_back<Default>::template get<keys::template find<T>>;
	
	template <std::size_t Idx>
	requires (Idx < size)
	using get_pair = std::pair<get_key<Idx>, get_value<get_key<Idx>>>;

	template <typename T>
	constexpr static inline const std::size_t find_key = keys::template find<T>;

	template <template <typename...> typename ConsumeT, typename... ConsumeAfter>
	using consume_p = ConsumeT<Pairs..., ConsumeAfter...>;

	template <template <typename...> typename ConsumeT, typename... ConsumeFirst>
	using consume_after_p = ConsumeT<ConsumeFirst..., Pairs...>;

	template <auto Lambda>
	constexpr static inline decltype(auto) consume_v = Lambda.template operator()<Pairs...>();

	template <IsSpecializationOf<std::pair>... Qairs>
	using put_p = _to_unique_pairs<Pairs..., Qairs...>::template consume_t<t_map>;

	template <IsSpecializationOf<std::pair>... Qairs>
	using put_if_absent_p = _to_unique_pairs<Qairs..., Pairs...>::template consume_t<t_map>;

	template <IsSpecializationOf<std::pair>... Qairs>
	using put_if_present_p = _to_unique_pairs<Pairs..., Qairs...>
		::template remove_l<_to_unique_pairs<Qairs..., Pairs...>>
		::template consume_t<put_p>;

	template <typename Key, typename Value>
	using put = put_p<std::pair<Key, Value>>;

	template <typename Key, typename Value>
	using put_if_absent = put_if_absent_p<std::pair<Key, Value>>;

	template <typename Key, typename Value, typename ValuePresent = get_value<Key>>
	using put_if_present = std::conditional<contains_key<Key> && std::is_same<get_value<Key>, ValuePresent>::value, put<Key, Value>, t_map<Pairs...>>::type;

	template <IsSpecializationOf<t_map> Map>
	using put_m = Map::template consume_p<put_p>;

	template <IsSpecializationOf<t_map> Map>
	using put_if_absent_m = Map::template consume_p<put_if_absent_p>;

	template <IsSpecializationOf<t_map> Map>
	using put_if_present_m = Map::template consume_p<put_if_present_p>;

	template <typename... Keys>
	using remove = pairs
		::template filter<[]<IsSpecializationOf<std::pair> Pair>{ return !t_list<Keys...>::template contains<typename Pair::first_type>; }>
		::template consume_t<t_map>;

	template <IsSpecializationOf<t_list> Keys>
	using remove_l = pairs
		::template filter<[]<IsSpecializationOf<std::pair> Pair>{ return !Keys::template contains<typename Pair::first_type>; }>
		::template consume_t<t_map>;

	template <IsSpecializationOf<std::pair>... Qairs>
	using remove_p = pairs
		::template filter<[]<IsSpecializationOf<std::pair> Pair>{ return !t_list<Qairs...>::template contains<Pair>; }>
		::template consume_t<t_map>;

	template <IsSpecializationOf<t_map>... Maps>
	using remove_m = pairs
		::template filter<[]<IsSpecializationOf<std::pair> Pair>{ return (true && ... && (!Maps::pairs::template contains<Pair>)); }>;
};