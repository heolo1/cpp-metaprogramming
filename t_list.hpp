#pragma once

#include <cstddef>
#include <type_traits>
#include <concepts>
#include "t_util.hpp"

template <typename... Ts>
struct t_list;

namespace {

template <std::size_t Idx, typename... Ts>
struct _t_get;

template <std::size_t Idx, typename First, typename... Ts>
requires (Idx == 0)
struct _t_get<Idx, First, Ts...> {
	using type = First;
};

template <std::size_t Idx, typename First, typename... Ts>
requires (Idx != 0)
struct _t_get<Idx, First, Ts...> {
	using type = _t_get<Idx - 1, Ts...>::type;
};

template <typename T, typename... Ts>
constexpr inline const std::size_t _t_find;

template <typename T>
constexpr inline const std::size_t _t_find<T> = 0;

template <typename T, typename First, typename... Ts>
constexpr inline const std::size_t _t_find<T, First, Ts...> = _t_find<T, Ts...> + 1;

template <typename T, std::same_as<T> First, typename... Ts>
constexpr inline const std::size_t _t_find<T, First, Ts...> = 0;

template <std::size_t, std::size_t, typename...>
struct _t_sub;

template <std::size_t From, std::size_t To, typename First, typename... Ts>
requires (From != 0 && To != 0)
struct _t_sub<From, To, First, Ts...> {
	using type = _t_sub<From - 1, To - 1, Ts...>::type;
};

template <std::size_t To, typename First, typename... Ts>
requires (To != 0)
struct _t_sub<0, To, First, Ts...> {
	using type = _t_sub<0, To - 1, Ts...>::type::template push_front<First>;
};

template <typename... Ts>
struct _t_sub<0, 0, Ts...> {
	using type = t_list<>;
};

template <typename...>
struct _is_homogeneous : std::false_type {};

template <>
struct _is_homogeneous<> : std::true_type {};

template <typename First, std::same_as<First>... Ts>
struct _is_homogeneous<First, Ts...> : _is_homogeneous<Ts...> {};

template <typename...>
struct _is_completely_heterogeneous : std::false_type {};

template <>
struct _is_completely_heterogeneous<> : std::true_type {};

template <typename First, IsNotSameAs<First>... Ts>
struct _is_completely_heterogeneous<First, Ts...> : _is_completely_heterogeneous<Ts...> {};

template <IsSpecializationOf<t_list>, typename...>
struct _t_remove;

template <IsSpecializationOf<t_list> List, typename T, typename... Rest>
struct _t_remove<List, T, Rest...> {
	using type = _t_remove<typename List::template remove_idx<List::template find<T>>>::type;
};

template <IsSpecializationOf<t_list> List>
struct _t_remove<List> {
	using type = List;
};

template <auto, typename...>
struct _t_filter_with_rest;

template <auto Lambda, typename First, typename... Rest>
struct _t_filter_with_rest<Lambda, First, Rest...> {
	using type = std::conditional<Lambda.template operator()<First, Rest...>(),
		typename _t_filter_with_rest<Lambda, Rest...>::type::template push_front<First>,
		typename _t_filter_with_rest<Lambda, Rest...>::type>::type;
};

template <auto Lambda>
struct _t_filter_with_rest<Lambda> {
	using type = t_list<>;
};

}

template <typename... Ts>
struct t_list {
	constexpr static inline const std::size_t size = sizeof...(Ts);

	template <std::size_t Idx>
	requires (Idx < size)
	using get = _t_get<Idx, Ts...>::type;

	template <typename T>
	constexpr static inline const std::size_t find = _t_find<T, Ts...>;

	template <typename T>
	constexpr static inline const bool contains = find<T> != size;

	template <typename... Us>
	using push_back = t_list<Ts..., Us...>;

	template <typename... Us>
	using push_front = t_list<Us..., Ts...>;

	template <std::size_t From, std::size_t To = size>
	requires (From <= To && To <= size)
	using sub = _t_sub<From, To, Ts...>::type;

	template <template <typename...> typename ConsumeT, typename... ConsumeAfter>
	using consume_t = ConsumeT<Ts..., ConsumeAfter...>;

	template <template <typename...> typename ConsumeT, typename... ConsumeFirst>
	using consume_after_t = ConsumeT<ConsumeFirst..., Ts...>;

	template <auto Lambda>
	constexpr static inline decltype(auto) consume_v = Lambda.template operator()<Ts...>();

	template <template <typename> typename MapT>
	using map_t = t_list<MapT<Ts>...>;

	template <auto BoolLambda>
	using filter_with_rest = _t_filter_with_rest<BoolLambda, Ts...>::type;

	template <auto BoolLambda>
	using filter = filter_with_rest<[]<typename T, typename...>{ return BoolLambda.template operator()<T>(); }>;

	template <IsSpecializationOf<t_list> List>
	using push_back_l = List::template push_front<Ts...>;

	template <IsSpecializationOf<t_list> List>
	using push_front_l = List::template push_back<Ts...>;

	template <std::size_t Idx, typename... Us>
	requires (Idx <= size)
	using push_idx = sub<0, Idx>::template push_back<Us...>::template push_back_l<sub<Idx>>;

	template <std::size_t Idx, IsSpecializationOf<t_list> List>
	requires (Idx <= size)
	using push_idx_l = sub<0, Idx>::template push_back_l<List>::template push_back_l<sub<Idx>>;

	template <std::size_t Idx, typename... Us>
	requires (Idx < size)
	using replace = sub<0, Idx>::template push_back<Us...>::template push_back_l<sub<Idx + 1>>;

	template <std::size_t From, std::size_t To = min_of<size, From + 1>>
	requires (From <= To && To <= size)
	using remove_idx = sub<0, From>::template push_back<sub<To>>;

	template <typename... Us>
	using remove = _t_remove<t_list<Ts...>, Us...>::type;

	template <IsSpecializationOf<t_list> List>
	using remove_l = List::template consume_t<remove>;

	constexpr static inline const bool is_homogenous = _is_homogeneous<Ts...>::value;

	constexpr static inline const bool is_completely_heterogeneous = _is_completely_heterogeneous<Ts...>::value;
};