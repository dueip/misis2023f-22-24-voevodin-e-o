#include <iostream>
#include <string>
#include <set>

int main() {
	std::set<char> input_name;
	char input;
	while (std::cin >> input) {
		input_name.insert(input);
		if (std::cin.peek() == '\n') break;
	}
	std::cout << (input_name.size() % 2 == 0 ? "CHAT WITH HER!" : "IGNORE HIM!");
}