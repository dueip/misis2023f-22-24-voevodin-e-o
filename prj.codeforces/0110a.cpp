#include <iostream>
#include <unordered_map>

int main() {
	std::unordered_map<char, int> bruh;
	std::string a;
	std::cin >> a;
	for (auto& el : a) {
		auto it = bruh.find(el);
		if (it == bruh.end())
			bruh.emplace(el, 1);
		else {
			it->second++;
		}
	}
	auto fours = bruh.find('4');
	auto sevens = bruh.find('7');
	if (fours == bruh.end() ) {
		if (sevens != bruh.end() && (sevens->second == 7 || sevens->second == 4)) {
			std::cout << "YES";
		}
		else {
			std::cout << "NO";
		}
		return 0;
	}
	if (sevens == bruh.end()) {
		if (fours != bruh.end() && (fours->second == 7 || fours->second == 4)) {
			std::cout << "YES";
		}
		else {
			std::cout << "NO";
		}
		return 0;
	}
	if ((fours->second + sevens->second) == 7 || (fours->second + sevens->second) == 4) {
		std::cout << "YES";
		return 0;
	}
	std::cout << "NO";
}	