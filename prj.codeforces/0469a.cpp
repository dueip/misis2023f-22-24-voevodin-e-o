#include <iostream>
#include <vector>
#include <array>

int main() {
	int n;
	std::cin >> n;
	std::vector<bool> ns(n + 1);
	int p;
	int q;
	std::cin >> p;
	for (int i = 0; i < p; ++i) {
		int el;
		std::cin >> el;
		ns[el] = true;
	}
	std::cin >> q;
	for (int i = 0; i < q; ++i) {
		int el;
		std::cin >> el;
		ns[el] = true;
	}
	for (auto i = ns.begin() + 1; i != ns.end(); ++i) {
		if (*i == false) {
			std::cout << "Oh, my keyboard!";
			return 0;
		}
	}
	std::cout << "I become the guy.";
}