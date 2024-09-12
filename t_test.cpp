#include <cstddef>
#include <iostream>
#include <utility>
#include <typeinfo>
#include <cxxabi.h>
#include <optional>
#include <type_traits>
#include "fixed_string.hpp"
#include "t_util.hpp"
#include "t_list.hpp"
#include "t_map.hpp"

namespace {

template <fixed_string, auto...>
struct _fs_as_list;

template <fixed_string fs, std::size_t... Idx, std::index_sequence<Idx...> idxseq>
struct _fs_as_list<fs, idxseq> {
	using type = t_list<std::integral_constant<char, fs[Idx]>...>;
};

template <IsSpecializationOf<t_list> List, IsSpecializationOf<t_map> Map>
struct _type_count_map;

template <IsSpecializationOf<t_list> List, IsSpecializationOf<t_map> Map>
requires (List::size != 0 && Map::template contains_key<typename List::template get<0>>)
struct _type_count_map<List, Map> {
	using key = List::template get<0>;
	using current = Map::template get_value<key>;
	using type = _type_count_map<typename List::template sub<1>,
		typename Map::template put<key, std::integral_constant<std::size_t, current::value + 1>>>::type;
};

template <IsSpecializationOf<t_list> List, IsSpecializationOf<t_map> Map>
requires (List::size == 0)
struct _type_count_map<List, Map> {
	using type = Map;
};

template <IsSpecializationOf<t_list> List, IsSpecializationOf<t_map> Map>
requires (List::size != 0 && !(Map::template contains_key<typename List::template get<0>>))
struct _type_count_map<List, Map> {
	using key = List::template get<0>;
	using type = _type_count_map<typename List::template sub<1>,
		typename Map::template put<key, std::integral_constant<std::size_t, 1>>>::type;
};

}

template <fixed_string fs>
using fs_as_list = _fs_as_list<fs, std::make_index_sequence<decltype(fs)::size>{}>::type;

template <IsSpecializationOf<t_list> List>
using type_count_map = _type_count_map<List, t_map<>>::type;

template <fixed_string fs>
using count_chars_in_string = type_count_map<fs_as_list<fs>>;

int main() {
	using char_map = count_chars_in_string<"the burger"_fs>;

	char_map::pairs::template consume_v<[]<IsSpecializationOf<std::pair>... Pairs>{
		return []{ std::cout << "| "; ((std::cout << Pairs::first_type::value << " -> " << Pairs::second_type::value << " | "), ...); std::cout << "\n"; };
	}>();
	std::cout << char_map::template get_value<std::integral_constant<char, 'r'>>::value << "\n";
}