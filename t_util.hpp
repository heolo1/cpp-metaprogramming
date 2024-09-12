#pragma once

#include <type_traits>
#include <concepts>

namespace {

template <typename, template <typename...> typename>
struct _is_specialization_of : std::false_type {};

template <template <typename...> typename TT, typename... Args>
struct _is_specialization_of<TT<Args...>, TT> : std::true_type {};

}

template <typename T, template <typename...> typename TT>
concept IsSpecializationOf = _is_specialization_of<T, TT>::value;

template <typename T1, typename T2>
concept IsNotSameAs = !std::same_as<T1, T2>;

template <auto...>
constexpr inline auto const max_of = std::unreachable_sentinel;

template <auto First, auto Second, auto... Elements>
constexpr inline auto const max_of<First, Second, Elements...> = max_of<(First > Second) ? First : Second, Elements...>;

template <auto Element>
constexpr inline const auto max_of<Element> = Element;

template <auto...>
constexpr inline auto const min_of = std::unreachable_sentinel;

template <auto First, auto Second, auto... Elements>
constexpr inline auto const min_of<First, Second, Elements...> = min_of<(First < Second) ? First : Second, Elements...>;

template <auto Element>
constexpr inline const auto min_of<Element> = Element;