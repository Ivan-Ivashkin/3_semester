#include <iostream>

template <typename t_data>
class Grid {
private:
	t_data* memory;
	size_t x_size, y_size;

public:

	Grid(size_t x_size, size_t y_size) : x_size{ x_size }, y_size{ y_size }, memory{ new t_data[x_size * y_size] } {}

	size_t get_xsize() const {
		return x_size;
	}

	size_t get_ysize() const {
		return y_size;
	}

	Grid(Grid const& old) : x_size{ old.get_xsize() }, y_size{ old.get_ysize() }, memory{ new t_data[x_size * y_size] } {
		for (size_t i = 0; i < x_size * y_size; ++i) {
			memory[i] = old[i];
		}
	}

	Grid& operator=(Grid const& old) {
		if (this == &old) {
			return *this;
		}		
		
		delete[] memory;

		x_size = old.get_xsize();
		y_size = old.get_ysize();

		this->memory = new t_data[x_size * y_size];

		for (size_t i = 0; i < x_size * y_size; ++i) {
			memory[i] = old[i];
		}

		return this;
	}

	~Grid() {
		delete[] memory;
	}

	t_data operator()(size_t x_idx, size_t y_idx) const {
		return memory[x_idx * x_size + y_idx];
	}

	t_data& operator()(size_t x_idx, size_t y_idx) {
		return memory[x_idx * x_size + y_idx];
	}

	friend std::ostream& operator<<(std::ostream& output, Grid const& grid) {
		for (size_t i = 0; i < grid.get_ysize(); ++i) {
			for (size_t j = 0; j < grid.get_xsize(); ++j) {
				output << grid.memory[i * grid.get_xsize() + j] << ' ';
			}
			output << '\n';
		}

		return output;		
	}

	friend std::istream& operator>>(std::istream& input, Grid& grid) {
		size_t i = 0;
		while (i < grid.get_xsize() * grid.get_ysize()) {
			input >> grid.memory[i];
			++i;
		}

		return input;
	}
};

int main(int argc, const char* argv[]) {

	//Grid<float> g(5, 5);
	//std::cin >> g;
	//std::cout << g;

	return 0;
}