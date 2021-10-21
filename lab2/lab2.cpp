#include <iostream>

class Handler {
	virtual void* data() = 0;

	virtual void const* data() const = 0;

	virtual std::type_info const& type() = 0;

	virtual ~Handler() = default;
};

template <typename T>
class TrivialHandler: public Handler {
private:
	T value;
public:
	void* data() override {
		return  static_cast<void*>(&value);
	}

	void const* data() const override {
		return  static_cast<void const*>(&value);
	}

	std::type_info type() override {
		return typeid(T);
	}
};

class Any {
private:
	Handler* h;
public:
	template <typename T>
	Any(T const &value) {
		h = new TrivialHandler<T>(value);
	}

	template <typename T>
	void replace(T const& value) {
		delete h;
		h = new TrivialHandler<T>(value);
	}

	template <typename T>
	T& as() {
		TrivialHandler<T>& th = dynamic_cast<TrivialHandler<T>&>(*h);
		return *(static_cast<T*>(th.data()));
	}

	template <typename T>
	T const& as() {
		TrivialHandler<T>& th = dynamic_cast<TrivialHandler<T>&>(*h);
		return *(static_cast<T const*>(th.data()));
	}

	template <typename T>
	bool is_contain() const {
		return h->type() == typeid(T);
	}
};

//dynamic_cast
//static_cast<void*>(&v)
//typeid(T) -> std::type_info
//ProxyRef

template <typename T>
class Grid {
private:
	T* memory;
	size_t x_size, y_size;

public:
	Grid(size_t x_size, size_t y_size) : x_size{ x_size }, y_size{ y_size }, memory{ new T[x_size * y_size] } {}

	size_t get_xsize() const {
		return x_size;
	}

	size_t get_ysize() const {
		return y_size;
	}

	Grid(Grid const& old) : x_size{ old.get_xsize() }, y_size{ old.get_ysize() }, memory{ new T[x_size * y_size] } {
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

		this->memory = new T[x_size * y_size];

		for (size_t i = 0; i < x_size * y_size; ++i) {
			memory[i] = old[i];
		}

		return this;
	}

	~Grid() {
		delete[] memory;
	}

	T operator()(size_t x_idx, size_t y_idx) const {
		return memory[x_idx * x_size + y_idx];
	}

	T& operator()(size_t x_idx, size_t y_idx) {
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

	Grid<float> g(2, 3);
	std::cin >> g;
	std::cout << g;

	return 0;
}