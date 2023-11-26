#include <iostream>
#include <vector>
#include <iterator>

//template<typename T>
//void printCollection(const std::iterator< beg, const std::iterator<std::contiguous_iterator_tag, T>& end) {
//	for (auto it = beg; it != end; ++it) {
//		std::cout << *it;
//	}
//}

int main() {
	int number;
	int width;
	std::vector<int> scores;
	std::cin >> number;
	std::cin >> width;
	for (int i = 0; i < number; ++i) {
		int score;
		std::cin >> score;
		scores.push_back(score);
	}

	for (auto& el : scores) {
		std::cout << el << " ";
	}
}