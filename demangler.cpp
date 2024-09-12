#include <iostream>
#include <cxxabi.h>
#include <utility>

const char *type_name = "_ZN5tableI6list_tIJXtl12fixed_stringILm6EEtlA7_cLc86ELc97ELc108ELc117ELc101ELc49EEEEXtlS2_tlS3_Lc86ELc97ELc108ELc117ELc101ELc50EEEEEEJ8gelementIS4_JXtlS1_ILm1EEtlA2_cLc97EEEEXtlS6_tlS7_Lc98EEEEEES5_IS4_JXtlS6_tlS7_Lc99EEEEXtlS1_ILm14EEtlA15_cLc115ELc111ELc109ELc101ELc116ELc104ELc105ELc110ELc103ELc32ELc101ELc108ELc115ELc101EEEEEES5_IS4_JXtlS1_ILm7EEtlA8_cLc101ELc108ELc101ELc109ELc101ELc110ELc116EEEEXtlS1_ILm4EEtlA5_cLc103ELc114ELc111ELc98EEEEEEEE9as_stringE";

int main() {
	std::size_t bufsize = 128;
	char *buf = reinterpret_cast<char *>(std::malloc(bufsize));
	int stat{};
	buf = abi::__cxa_demangle(type_name, nullptr, nullptr, &stat);
	if (stat) {
		std::cerr << "Returned code " << stat << "\n";
	}
	std::cout << "Mangled: " << type_name << "\n";
	std::cout << "Unmangled:" << buf << std::endl;
	std::free(buf);
	return stat;
}