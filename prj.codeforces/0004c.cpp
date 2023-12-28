#include <iostream>
#include <unordered_set>
#include <set>
#include <map>
#include <unordered_map>
#include <string>
#include <algorithm>

int main() {
	int n;
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);
	std::cin >> n;
	std::unordered_set<int, std::string> hash;
	std::set<std::string> hash_unique;
	for (int i = 0; i < n; ++i) {
		std::string thingie;
		std::cin >> thingie;
		hash.emplace(i, thingie);
		hash_unique.insert(thingie);
	}
	int count = 0;
	for (auto& el : hash_unique) {
		while (hash.contains(el)) {
			hash.extract(el);
			if (count > 0) {
				std::cout << el + std::to_string(count) << "\n";
			}
			else {
				std::cout << "OK\n";
			}
			++count;
		}
		count = 0;
	}
}