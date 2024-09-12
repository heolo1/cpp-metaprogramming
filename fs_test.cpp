#include <iostream>
#include <string>
#include "fixed_string.hpp"
#include "list_t.hpp"
#include "fs_table.hpp"

using t_columns = list_t<"Value1"_fs, "Value2"_fs>;
template <fixed_string... Values>
using t_element = gelement<t_columns, Values...>;
static_assert(IsFixedStringList<t_columns>);
using t_table = table<t_columns, t_element<"a"_fs, "b"_fs>, t_element<"c"_fs, "something else"_fs>, t_element<"element"_fs, "grob"_fs>>;

int main() {
	std::cout << t_table::as_string << "\n";
	return 0;
}