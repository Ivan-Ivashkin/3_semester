#include <iostream>
#include <set>
#include <random>
#include <vector>

class State {
public:
    virtual bool contains(int s) const = 0;

    virtual ~State() = default;
};

class DiscreteState : public State {
private:
    int const state;

public:
    DiscreteState(int state) : state(state) { }

    bool contains(int s) const override {
        return s == state;
    }
};

class SegmentState : public State {
private:
    int const beg, end;

public:
    SegmentState() : beg(0), end(-1) { }
    SegmentState(int beg, int end) : beg(beg), end(end) { }

    bool contains(int s) const override {
        return s >= beg && s <= end;
    }
};

class ContGaps : public State { // непрерывные с пропусками
private:
    std::vector<SegmentState> cont;
    std::vector<DiscreteState> gaps;
public:
    ContGaps(std::vector<SegmentState> cont, std::vector<DiscreteState> gaps) : cont(cont), gaps(gaps) { }

    bool contains(int s) const override {
        for (auto const& e : gaps) {
            if (e.contains(s)) {
                return false;
            }
        }
        for (auto const& e : cont) {
            if (e.contains(s)) {
                return true;
            }
        }
        return false;
    }
};

class ContAdds : public State { // непрерывные с дополнениями
private:
    std::vector<SegmentState> cont;
    std::vector<DiscreteState> adds;
public:
    ContAdds(std::vector<SegmentState> cont, std::vector<DiscreteState> adds) : cont(cont), adds(adds) { }

    bool contains(int s) const override {
        for (auto const& e : adds) {
            if (e.contains(s)) {
                return true;
            }
        }
        for (auto const& e : cont) {
            if (e.contains(s)) {
                return true;
            }
        }
        return false;
    }
};

class ContGapsAdds : public State { // непрерывные с пропусками и дополнениями
private:
    std::vector<SegmentState> cont;
    std::vector<DiscreteState> adds;
    std::vector<DiscreteState> gaps;
public:
    ContGapsAdds(std::vector<SegmentState> cont, std::vector<DiscreteState> adds, std::vector<DiscreteState> gaps) : cont(cont), adds(adds), gaps(gaps) { }

    bool contains(int s) const override {
        for (auto const& e : gaps) {
            if (e.contains(s)) {
                return false;
            }
        }
        for (auto const& e : adds) {
            if (e.contains(s)) {
                return true;
            }
        }
        for (auto const& e : cont) {
            if (e.contains(s)) {
                return true;
            }
        }
        return false;
    }
};

class UnionState : public State { // объединение
private:
    State* s1;
    State* s2;
public:
    UnionState(State* s1, State* s2) : s1(s1), s2(s2) { }

    bool contains(int s) const {
        return s1->contains(s) || s2->contains(s);
    }
};

class IntersectionState : public State { // пересечение
private:
    State* s1;
    State* s2;
public:
    IntersectionState(State* s1, State* s2) : s1(s1), s2(s2) { }

    bool contains(int s) const {
        return s1->contains(s) && s2->contains(s);
    }
};

class SetState : public State {
private:
    std::set<int> const states;

public:
    SetState() : states() { }
    SetState(std::set<int> const& src) : states(src) { }

    bool contains(int s) const override {
        return states.count(s) > 0;
    }
};

class ProbabilityTest {
private:
    unsigned seed;
    int test_min, test_max;
    unsigned test_count;

public:
    ProbabilityTest(unsigned seed, int test_min, int test_max, unsigned test_count) : seed(seed), test_min(test_min), test_max(test_max), test_count(test_count) { }

    float operator()(State const& s) const {
        std::default_random_engine rng(seed);
        std::uniform_int_distribution<int> dstr(test_min, test_max);
        unsigned good = 0;
        for (unsigned cnt = 0; cnt != test_count; ++cnt)
            if (s.contains(dstr(rng))) ++good;

        return static_cast<float>(good) / static_cast<float>(test_count);
    }
};

class Factory {
public:
    static State* create_ContGaps(std::vector<SegmentState> cont, std::vector<DiscreteState> gaps) {
        return new ContGaps(cont, gaps);
    }
    static State* create_ContAdds(std::vector<SegmentState> cont, std::vector<DiscreteState> adds) {
        return new ContGaps(cont, adds);
    }
    static State* create_ContGapsAdds(std::vector<SegmentState> cont, std::vector<DiscreteState> adds, std::vector<DiscreteState> gaps) {
        return new ContGapsAdds(cont, adds, gaps);
    }
    static State* create_UnionState(State *s1, State *s2) {
        return new UnionState(s1, s2);
    }
    static State* create_IntersectionState(State* s1, State* s2) {
        return new IntersectionState(s1, s2);
    }

    static void release(State* ptr) {
        delete ptr;
    }
};

void test1() {
    SegmentState s(0, 50);

    for (int n = 1; n <= 1000; n++) {
        ProbabilityTest pt(2021, 0, 100, n);
        std::cout << pt(s) << std::endl;
    }
}

void test2() {
    SegmentState s1(0, 10);
    SegmentState s2(10, 30);

    State* ss1 = Factory::create_UnionState(&s1, &s2);

    for (int n = 1; n <= 1000; n++) {
        ProbabilityTest pt(2021, 0, 60, n);
        std::cout << pt(*ss1) << std::endl;
    }

    Factory::release(ss1);
}

void test3() {
    SetState s1({ 7, 27, 3, 14, 22, 25, 1, 8, 9, 20, 30, 12, 2, 4, 15, 16, 29, 18, 19, 10, 21, 5, 23, 24, 6, 26, 13, 28, 17, 11 });
    for (int n = 1; n <= 1000; n++) {
        ProbabilityTest pt(2021, 0, 60, n);
        std::cout << pt(s1) << std::endl;
    }
}

int main(int argc, const char* argv[]) {
    //test1();
    test2();
    //test3();

    return 0;
}