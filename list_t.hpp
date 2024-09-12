#pragma once

#include <iterator>
#include <stdexcept>

template <size_t Idx, auto... Elements>
constexpr inline const auto element_at = [](){ throw std::out_of_range("Index is out of range"); }();

template <size_t Idx, auto First, auto... Rest>
requires (Idx == 0)
constexpr inline const auto element_at<Idx, First, Rest...> = First;

template <size_t Idx, auto First, auto... Rest>
requires (Idx != 0)
constexpr inline const auto element_at<Idx, First, Rest...> = element_at<Idx - 1, Rest...>;

namespace {

template <size_t, typename...>
struct _element_t_at;

template <size_t Idx, typename First, typename... Rest>
requires (Idx == 0)
struct _element_t_at<Idx, First, Rest...> {
	using type = First;
};

template <size_t Idx, typename First, typename... Rest>
requires (Idx != 0)
struct _element_t_at<Idx, First, Rest...> {
	using type = _element_t_at<Idx - 1, Rest...>::type;
};

}

template <size_t Idx, typename... Ts>
using element_t_at = _element_t_at<Idx, Ts...>::type;

namespace {

template <auto, std::size_t, auto...>
constexpr inline auto _index_of = std::unreachable_sentinel;

template <auto Element, std::size_t Idx>
constexpr inline auto _index_of<Element, Idx> = Idx;

template <auto Element, std::size_t Idx, auto First, auto... Rest>
requires (Element == First)
constexpr inline auto _index_of<Element, Idx, First, Rest...> = Idx;

template <auto Element, std::size_t Idx, auto First, auto... Rest>
requires (Element != First)
constexpr inline auto _index_of<Element, Idx, First, Rest...> = _index_of<Element, Idx + 1, Rest...>;

} // hide underlying definitions for index_of

template <auto Element, auto... Rest>
constexpr inline auto const index_of = _index_of<Element, 0, Rest...>;

template <auto... Elements>
constexpr inline auto const max = std::unreachable_sentinel;

template <auto First, auto Second, auto... Elements>
constexpr inline auto const max<First, Second, Elements...> = max<(First > Second) ? First : Second, Elements...>;

template <auto Element>
constexpr inline const auto max<Element> = Element;

template <auto...>
constexpr inline const auto are_same_type = false;

template <>
constexpr inline const auto are_same_type<> = true;

template <auto First, decltype(First)... Rest>
constexpr inline const auto are_same_type<First, Rest...> = true;

template <auto... Elements>
struct list_t {
	constexpr static inline const std::size_t size = sizeof...(Elements);
	template <size_t Idx>
	constexpr static inline const auto element = element_at<Idx, Elements...>;
	
	constexpr static inline const auto is_homogenous = are_same_type<Elements...>;

	template <auto Element>
	constexpr static inline const std::size_t index_of = ::index_of<Element, Elements...>;

	template <auto Element>
	constexpr static inline const auto contains = index_of<Element> != sizeof...(Elements);

	template <auto ElementTemplateLambda>
	constexpr static inline const auto apply = ElementTemplateLambda.template operator()<Elements...>();
};

namespace {

template <typename>
constexpr inline const auto is_list_t_v = false;

template <auto... Elements>
constexpr inline const auto is_list_t_v<list_t<Elements...>> = true;

}

template <typename T>
concept IsList = is_list_t_v<T>;