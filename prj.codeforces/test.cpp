#include <iostream>
#include <array>

int main(int argc, char const* argv[])
{
	std::array<int, 9> I { 0,0,0,1,1,3,3,4,4 };
	std::array<int, 9> J { 1,2,5,2,4,5,2,3,5 };
	std::array<int, 9> L {-1,-1,-1,-1,-1,-1,-1,-1,-1};
	std::array<int, 6> H {-1,-1,-1,-1,-1,-1};

	for(int k = 0; k < I.size(); k++){
		int i = I[k];
		L[k] = H[i];
	 	H[i] = k;
	 }

	 for(int i = 0; i < H.size(); i++){
	 	std::cout << H[i] << " ";
	 }
	 std::cout << "\n";
	 
	  for(int i = 0; i < L.size(); i++){
	 	std::cout << L[i] << " ";
	}
	std::cout << "\n";
	//return 0;
	//int L[10] = { -1,0,-1,-1,3,4,-1,6,7,-1 };
	//int H[6] = { 1,2,5,8,9,-1 };

	std::array<int, 6> R = { 0,6,6,6,6,6 };
	std::array<int, 6> P = { -1,-2,-2,-2,-2,-2 };
	std::array<int, 6> Q = { 0,-2,-2,-2,-2,-2 };
	int r = 0;
	int w = 1;
	int count = 0;


	while (r < w) {
		int i = Q[r];
		r++;
		for (int k = H[i]; k != -1; k = L[k]) {
			int j = J[k];
			if (R[j] == 6) {
				R[j] = R[i] + 1;
				P[j] = k;
				Q[w] = j;
				w++;
			}
		}
		std::cout << count << ":\nP: ";
		for (int i = 0; i < P.size(); i++) {
			std::cout << P[i] << " ";
		}
		std::cout << "\n\nR: ";
		for (int i = 0; i < R.size(); i++) {
			std::cout << R[i] << " ";
		}
		std::cout << "\n\nQ: ";
		for (int i = 0; i < Q.size(); i++) {
			std::cout << Q[i] << " ";
		}
		std::cout << "\n\nr: " << r << "   w: " << w << "\n\n";
		count++;
	}

	return 0;
}

// void print_result(int P_[6], int R_[6], int Q_[6], int &r_, int &w_, int &count_){
// 	std::cout << count_ << ":\nP: ";
// 	for(int i = 0; i < 6; i++){
// 		std::cout << P_[i] << " ";
// 	}
// 	std::cout << "\n\nR: ";
// 	for(int i = 0; i < 6; i++){
// 		std::cout << R_[i] << " ";
// 	}
// 	std::cout << "\n\nQ: ";
// 	for(int i = 0; i < 6; i++){
// 		std::cout << Q_[i] << " ";
// 	}
// 	std::cout << "\n\nr: " << r_ << "   w: "<<  w_ << "\n\n";
// }