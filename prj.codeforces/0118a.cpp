#include <iostream>
#include <string>
#include <array>

constexpr [[nodiscard]] bool isVowel(const char character) {
	constexpr std::array<char, 6> vowels = { 'a', 'o', 'y', 'e', 'u', 'i'};
	for (char el : vowels) {
		if (el == character) return true;
	}
	return false;
}

constexpr [[nodiscard]] bool isConsonant(const char character) {
	return !(isVowel(character));
}

int main() {
	std::string input_string;
	// Easier to work with 2 strings than modify the input string
	std::string output_string;
	std::cin >> input_string;

	for (const char el : input_string) {
		if (isConsonant(std::tolower(el))) {
			output_string.push_back('.');
			output_string.push_back(std::tolower(el));
		}
	}
	std::cout << output_string;
}