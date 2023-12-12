#include <iostream>

int main() {
	int n;
	std::cin >> n;
	int x = 0;
	for (int i = 0; i < n; ++i) {
		std::string str;
		std::cin >> str;
		if (str.find_first_of("+") != str.npos) ++x;
		else --x;
	}
	std::cout << x;
}