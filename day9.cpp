#include <iostream>
#include <vector>

class Map {
private:
	int N, M;
	std::vector<std::vector<int>> map;
public:
	Map() {

	}

	Map(int N, int M): N(N+2), M(M+2) {
		for (int i = 0; i <= N+1; i++) {
			std::vector<int> arr;
			for (int j = 0; j <= M+1; j++) {
				arr.push_back(-1);
			}
			map.push_back(arr);
		}
	};

	void add(int i, int j, int value) {
		map[i][j] = value;
	}

	int check(int i, int j) {
		if ((map[i][j] < map[i - 1][j]) && (map[i][j] < map[i + 1][j]) && (map[i][j] < map[i][j - 1]) && (map[i][j] < map[i][j + 1])) {
			return map[i][j];
		} else {
			return -1;
		}
	}

	void print() {
		for (int i = 1; i < N-1; i++) {
			for (int j = 1; j < M-1; j++) {
				std::cout << map[i][j] << ' ';
			}
			std::cout << std::endl;
		}
	}
};

int main(){
	int N, M;
	std::cin >> N;
	std::cin >> M;

	Map cave(N, M);

	int max = -1;

	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			int a;
			std::cin >> a;
			if (a > max) {
				max = a;
			}
			cave.add(i, j, a);
		}
	}

	for (int j = 0; j < M+2; j++) {
		cave.add(0, j, max);
		cave.add(N+1, j, max);
	}

	for (int i = 0; i < N+2; i++) {
		cave.add(i, 0, max);
		cave.add(i, M+1, max);
	}

	std::cout << std::endl;
	cave.print();
	std::cout << std::endl;

	int s;
	s = 0;
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			int level = cave.check(i, j);
			if (level != -1) {
				std::cout << level << std::endl;
				s = s + level + 1;
			}
		}
	}
	std::cout << std::endl << s << std::endl;

return 0;

}