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

int main(int argc, const char* argv[]) {
    DiscreteState d(1);
    ProbabilityTest pt(10, 0, 100, 100000);

    std::cout << pt(d) << std::endl;

    return 0;
}