#include <iostream>
#include <vector>
#include <string>



int main() {
	const std::string hello("hello");
	std::string input;
	std::cin >> input;
	std::vector<std::vector<int>> hello_mapping;
	hello_mapping.resize(input.size());
	for (auto& el : hello_mapping) {
		el.resize(hello.size());
	}

	int max = -1;

	for (int i = 0; i < hello_mapping.size(); ++i) {
		for (int j = 0; j < hello_mapping[i].size(); ++j) {
			bool already_carried_one = false;
			

			if (input[i] == hello[j]) {
				if (i > 0) {
					if (hello_mapping[i - 1][j] > 0) {
						
						if ((j > 0 && hello_mapping[i - 1][j - 1] == 0) || j == 0) {
							hello_mapping[i][j] = hello_mapping[i - 1][j];
							already_carried_one = true;
						}
						//continue; 
					}
				}
				if (i > 0 && j > 0) {
					
					hello_mapping[i][j] += hello_mapping[i - 1][j - 1];
				}
				if (!already_carried_one) {
					hello_mapping[i][j] += 1;
				}
				max = std::max(hello_mapping[i][j], max);
				if (max == hello.size()) {
					std::cout << "YES";
					return 0;
				}
			}
			else {
				if (i > 0) {
					hello_mapping[i][j] = hello_mapping[i - 1][j];
				}
			}
		}
	}
	std::cout << "NO";


}