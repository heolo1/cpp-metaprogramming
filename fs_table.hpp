#pragma once

#include <concepts>
#include <utility>
#include <type_traits>
#include "fixed_string.hpp"
#include "list_t.hpp"

template <typename List>
concept IsFixedStringList = IsList<List> && List::template apply<[]<auto... Elements>{ return (true && ... && IsFixedString<std::remove_cvref_t<decltype(Elements)>>); }>;

template <typename Entry, fixed_string Column>
concept HasElement = requires {
	{ Entry::template element<Column> } -> fixed_string;
};

template <typename Entry, typename Columns>
concept HasElements = IsFixedStringList<Columns> && []<std::size_t... Idx>(std::index_sequence<Idx...> &&) {
	return (true && ... && HasElement<Entry, Columns::template element<Idx>>);
}(std::make_index_sequence<Columns::size>{});

template <IsFixedStringList Columns, HasElements<Columns>... Entries>
struct table {
	template <std::size_t Idx>
	constexpr static inline const auto column = Columns::template element<Idx>;
	
	template <std::size_t Idx>
	using entry = element_t_at<Idx, Entries...>;

	template <fixed_string Column>
	requires Columns::template contains<Column>
	constexpr static inline const std::size_t max_width = max<decltype(Column)::size, decltype(Entries::template element<Column>)::size...>;

	template <HasElements<Columns> Entry>
	constexpr static inline const auto entry_string = "|"_fs + Columns::template apply<[]<auto... Column>{
		return ((" "_fs + Entry::template element<Column> + " "_fs.template repeat<max_width<Column> - decltype(Entry::template element<Column>)::size>() + " |"_fs) + ...);
	}>;

	constexpr static inline const auto column_string = "|"_fs + Columns::template apply<[]<auto... Column>{
		return ((" "_fs + Column + " "_fs.template repeat<max_width<Column> - decltype(Column)::size>() + " |"_fs) + ...);
	}>;

	constexpr static inline const auto border_string = "+"_fs + Columns::template apply<[]<auto... Column>{
		return (("-"_fs.template repeat<2 + max_width<Column>>() + "+"_fs) + ...);
	}>;

	constexpr static inline const auto as_string = border_string + "\n"_fs + column_string + "\n"_fs + border_string
		+ (("\n"_fs + entry_string<Entries>) + ... + ""_fs) + "\n"_fs + border_string;
};

template <IsFixedStringList Columns, fixed_string... Values>
requires (Columns::size == sizeof...(Values))
struct gelement {
	template <fixed_string Column>
	constexpr static inline const auto element = element_at<Columns::template index_of<Column>, Values...>;
};