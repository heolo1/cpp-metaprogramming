#include <iostream>
#include <utility>
#include <array>

std::ostream &operator<<(std::ostream &os, const std::array<auto, auto> &arr) {
	return [&]<std::size_t... Idx>(std::index_sequence<Idx...>) -> std::ostream & {
		return (os << ... << arr[Idx]);
	}(std::make_index_sequence<std::tuple_size_v<decltype(arr)>>{});
}

int main() {
	int nums[] = {1, 2, 3, 4, 5, 6};
	std::cout << __cplusplus << " " << nums << "\n";
}