#include <iostream>
#include <vector>

class DemoStr {
public:
    DemoStr() : data_{ nullptr }, length_{ 0 }, capacity_{ length_ }{
    }

    DemoStr(DemoStr const& other) : length_{ other.length_ }, capacity_{ length_ }{
        allocate(other.length_);
        copy_str(other.data_, data_, other.length_);
    }

    DemoStr(char const* raw_str, size_t length) : length_{ length }, capacity_{ length_ }{
        allocate(length);
        copy_str(raw_str, data_, length);
        //reformat();
    }

    explicit DemoStr(int value) {
        length_ = get_value_length(value);
        capacity_ = length_;
        allocate(length_);
        auto it = &data_[length_ - 1];
        while (value > 0) {
            *it = value % 10 + '0';
            --it;
            value /= 10;
        }
    }

    void print() const {
        print(std::cout);
    }

    void print(std::ostream& out) const {
        for (auto i = 0u; i < length_; ++i)
            out << data_[i];
        //out << "\n";
    }

    //index <= length_ - 1
    char at(size_t index) const {
        return data_[index];
    }

    bool equal(DemoStr const& other) const {
        if (other.length_ != length_)
            return false;
        for (auto i = 0u; i < length_; ++i)
            if (data_[i] != other.data_[i])
                return false;

        return true;
    }

    void read_from(std::istream& in) {
        char tmp;
        while (in >> tmp) {
            if (length_ >= capacity_)
                reallocate((capacity_ == 0) ? 1 : capacity_ * 2);
            data_[length_] = tmp;
            ++length_;
        }
        reformat();
    }

    void shrink_to_fit() {
        reallocate(length_);
    }

    void clear() {
        delete[] data_;
        data_ = nullptr;
        length_ = capacity_ = 0;
    }

    size_t size() {
        return length_;
    }

    ~DemoStr() {
        clear();
    }

protected:
    void reallocate(size_t n) {
        auto old_data = data_;

        data_ = new char[n];
        capacity_ = n;

        copy_str(old_data, data_, std::min(n, length_));

        delete[] old_data;
    }

    void reformat() {
        for (auto i = 0u; i < length_; ++i)
            data_[i] = (i % 2 == 1) ? std::tolower(data_[i]) :
            std::toupper(data_[i]);
    }

    size_t get_value_length(int value) {
        auto len = 0ull;
        while (value > 0) {
            value /= 10;
            ++len;
        }
        return len;
    }

    void allocate(size_t n) {
        data_ = new char[n];
    }

    void copy_str(char const* str, char* dst, size_t n) {
        for (auto i = 0u; i < n; ++i)
            dst[i] = str[i];
    }

    void set_length(size_t n) {
        length_ = n;
    }

    char* data() {
        return data_;
    }

    //friend std::ostream& operator << (std::ostream& out, DemoStr const& v);
private:
    char* data_ = nullptr;
    size_t length_ = 0, capacity_ = 0;
};

std::ostream& operator << (std::ostream& out, DemoStr const& v) {
    v.print(out);
    /*for (auto i = 0ul; i < v.length_; ++i)
        out << v.data_[i];
    out << "\n";*/
    return out;
}

std::istream& operator >> (std::istream& in, DemoStr& v) {
    v.clear();
    v.read_from(in);
    return in;
}

class MyStr: public DemoStr {
public:
    using DemoStr::DemoStr;

    void append(MyStr tail) {
        size_t new_size = this->size() + tail.size();
        this->reallocate(new_size);
        copy_str(tail.data(), this->data() + this->size(), tail.size());
        this->set_length(new_size);
    }

    std::vector<MyStr> split(char sym) {

        std::vector<MyStr> list;
        MyStr mem;

        for (size_t i = 0; i < this->size(); ++i) {
            if ( (this->data()[i] == sym) || (i == this->size() - 1) ) {
                list.push_back(mem);
                mem.clear();
            }
            else {
                MyStr c(&this->data()[i], 1);
                mem.append(c);
            }
        }

        return list;
    }
};


int main(int argc, char** argv) {
    char demo1[] = "Hello,";
    MyStr str1(demo1, sizeof(demo1) - 1);

    char demo2[] = " world!";
    MyStr str2(demo2, sizeof(demo2) - 1);

    std::cout << "Append:" << std::endl;
    str1.append(str2);
    str1.print();

    std::cout << std::endl << "Split:" << std::endl;

    std::vector<MyStr> parts;
    parts = str1.split('l');

    for (auto const& e : parts)
        std::cout << e << ' ';
    std::cout << std::endl;

    //std::cout << str << "\n";
    //str.clear();
    //std::cout << "Empty str?:" << std::boolalpha << str.equal(DemoStr()) << "\n";

    //std::cin >> str;
    //str.shrink_to_fit();
    //std::cout << "\nRead data: " << str;

    return 0;
}