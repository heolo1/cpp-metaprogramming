#pragma once

#include <cstddef>
#include <iostream>
#include <tuple>
#include <utility>
#include <type_traits>
#include <string_view>

template <std::size_t N>
struct fixed_string {
	constexpr static const std::size_t size = N;
	constexpr static const bool is_empty = size == 0;

	constexpr fixed_string() = default;

	constexpr fixed_string(const char (&str)[N + 1]) noexcept {
		[&]<std::size_t... NIdx>(std::index_sequence<NIdx...>) {
			((_data[NIdx] = str[NIdx]), ...);
		}(std::make_index_sequence<N>{});
	}
	
	friend constexpr auto operator<=>(const fixed_string &, const fixed_string &) = default;
	friend constexpr auto operator==(const fixed_string &, const fixed_string &) -> bool = default;
	
	constexpr operator const char *() {
		return _data;
	}

	constexpr auto data() const & noexcept -> const char (&)[N+1] {
		return _data;
	}
	
	constexpr auto data() & noexcept -> char (&)[N+1] {
		return _data;
	}
	
	constexpr auto begin() const & noexcept -> char const * {
		return _data;
	}
	
	constexpr auto end() const & noexcept -> char const * {
		return _data + N;
	}
	
	constexpr auto begin() & noexcept -> char * {
		return _data;
	}
	
	constexpr auto end() & noexcept -> char * {
		return _data + N;
	}
	
	constexpr auto &operator[](std::size_t index) noexcept {
		return _data[index];
	}

	constexpr auto &operator[](std::size_t index) const noexcept {
		return _data[index];
	}
	
	char _data[N + 1]{};

	template <std::size_t Count>
	constexpr fixed_string<N * Count> repeat() const noexcept {
		return [&]<std::size_t... CountIdx, std::size_t... NIdx>(fixed_string<N * Count> &&fs, std::index_sequence<CountIdx...>, std::index_sequence<NIdx...>) {
			([&](std::size_t n_value){ (fs[n_value + CountIdx * N] = ... = _data[n_value]); }(NIdx), ...);
			return fs;
		}({}, std::make_index_sequence<Count>{}, std::make_index_sequence<N>{});
	}
};

template <std::size_t N>
fixed_string(const char (&str)[N]) -> fixed_string<N - 1>;

template <fixed_string fs>
consteval auto operator""_fs() {
	return fs;
}

template <std::size_t N>
std::ostream &operator<<(std::ostream &os, const fixed_string<N> &fs) {
	return os << std::string_view{fs.data()};
}

template <std::size_t A, std::size_t B>
constexpr fixed_string<A + B> operator+(const fixed_string<A> &a, const fixed_string<B> &b) {
	return [&]<std::size_t... IdxA, std::size_t... IdxB>
		(fixed_string<A + B> &&fs, std::index_sequence<IdxA...>, std::index_sequence<IdxB...>) -> fixed_string<A + B> {
		((fs[IdxA] = a[IdxA]), ...);
		((fs[A + IdxB] = b[IdxB]), ...);
		return fs;
	}({}, std::make_index_sequence<A>{}, std::make_index_sequence<B>{});
}

namespace {

template <typename>
constexpr inline const auto is_fixed_string_v = false;

template <std::size_t N>
constexpr inline const auto is_fixed_string_v<fixed_string<N>> = true;

}

template <typename T>
concept IsFixedString = is_fixed_string_v<T>;