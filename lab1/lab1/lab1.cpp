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

    bool contains(int s) const override {
        return s1->contains(s) || s2->contains(s);
    }
};

class IntersectionState : public State { // пересечение
private:
    State* s1;
    State* s2;
public:
    IntersectionState(State* s1, State* s2) : s1(s1), s2(s2) { }

    bool contains(int s) const override {
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
        return new ContAdds(cont, adds);
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

class Tester { // некоторые тесты - с элементом случайности, остальные реализованы проще - на конкретных числах
public:
    bool static test_DiscreteState() {
        std::default_random_engine rng(1903);
        std::uniform_int_distribution<int> dstr(0, 100);
        int test_value = dstr(rng);

        DiscreteState d(test_value);
        for (int i = 0; i <= 100; i++) {
            if (d.contains(i) && !(i == test_value)) {
                return false;
            }
        }

        return true;
    }

    bool static test_SegmentState() {
        std::default_random_engine rng(1917);
        std::uniform_int_distribution<int> dstr(0, 100);
        int min = dstr(rng);

        SegmentState s(min, 100);
        for (int i = 0; i <= 100; i++) {
            if (s.contains(i) && !((i >= min) && (i <= 100))) {
                return false;
            }
        }

        return true;
    }

    bool static test_ContGaps() {
        SegmentState s1(0, 10);
        SegmentState s2(20, 30);
        SegmentState s3(40, 50);

        DiscreteState d1(7);
        DiscreteState d2(25);
        DiscreteState d3(41);

        std::vector<SegmentState> cont = {s1, s2, s3};
        std::vector<DiscreteState> gaps = {d1, d2, d3};

        ContGaps cg(cont, gaps);

        std::vector<int> tests = { 2, 7, 29, 25, 50, 1, 41 };
        std::vector<bool> results;
        std::vector<bool> control = { true, false, true, false, true, true, false };

        for (auto& e : tests) {
            results.push_back(cg.contains(e));
        }

        if (results == control) {
            return true;
        }

        return false;
    }

    bool static test_ContAdds() {
        SegmentState s1(0, 10);
        SegmentState s2(20, 30);
        SegmentState s3(40, 50);

        DiscreteState d1(13);
        DiscreteState d2(25);
        DiscreteState d3(69);

        std::vector<SegmentState> cont = { s1, s2, s3 };
        std::vector<DiscreteState> adds = { d1, d2, d3 };

        ContAdds ca(cont, adds);

        std::vector<int> tests = { 13, 25, 69, 100, 3, 37};
        std::vector<bool> results;
        std::vector<bool> control = { true, true, true, false, true, false};

        for (auto& e : tests) {
            results.push_back(ca.contains(e));
        }

        if (results == control) {
            return true;
        }

        return false;
    }

    bool static test_ContGapsAdds() {
        SegmentState s1(0, 10);
        SegmentState s2(20, 30);
        SegmentState s3(40, 50);

        DiscreteState d1(13);
        DiscreteState d2(25);
        DiscreteState d3(69);

        DiscreteState d4(1);
        DiscreteState d5(29);
        DiscreteState d6(41);

        std::vector<SegmentState> cont = { s1, s2, s3 };
        std::vector<DiscreteState> adds = { d1, d2, d3 };
        std::vector<DiscreteState> gaps = { d4, d5, d6 };

        ContGapsAdds cga(cont, adds, gaps);

        std::vector<int> tests = { 13, 25, 69, 1, 29, 41, 100, 7, 82, 21, 32};
        std::vector<bool> results;
        std::vector<bool> control = { true, true, true, false, false, false, false, true, false, true, false};

        for (auto& e : tests) {
            results.push_back(cga.contains(e));
        }

        if (results == control) {
            return true;
        }

        return false;
    }

    bool static test_SetState() {
        SetState ss({ 1, 2, 7, 10, 23, 34, 19, 83, 100, 77 });

        std::vector<int> tests = { 83, 100, 5, 77, 2, 1, 20, 35, 4, 10 };
        std::vector<bool> results;
        std::vector<bool> control = { true, true, false, true, true, true, false, false, false, true};

        for (auto& e : tests) {
            results.push_back(ss.contains(e));
        }

        if (results == control) {
            return true;
        }

        return false;
        
    }

    bool static test_UnionState() {
        std::default_random_engine rng(1895);
        std::uniform_int_distribution<int> dstr(0, 100);
        int t1 = dstr(rng); int t2 = dstr(rng);
        int t3 = dstr(rng); int t4 = dstr(rng);

        DiscreteState d1(t1);
        DiscreteState d2(t2);

        SegmentState s1(fmin(t1, t2), fmax(t1, t2));
        SegmentState s2(fmin(t3, t4), fmax(t3, t4));

        UnionState u1(&d1, &d2);
        UnionState u2(&s1, &s2);

        for (int i = 0; i <= 100; i++) {
            if (u1.contains(i) && !(i == t1 || i == t2)) {
                return false;
            }

            if (u2.contains(i) && !( (i >= fmin(t1, t2) && i <= fmax(t1, t2)) || (i >= fmin(t3, t4) && i <= fmax(t3, t4))) ) {
                return false;
            }
        }

        return true;
    }

    bool static test_IntersectionState() {
        std::default_random_engine rng(1914);
        std::uniform_int_distribution<int> dstr(0, 100);
        int t1 = dstr(rng); int t2 = dstr(rng);
        int t3 = dstr(rng); int t4 = dstr(rng);

        DiscreteState d1(t1);
        DiscreteState d2(t2);

        SegmentState s1(fmin(t1, t2), fmax(t1, t2));
        SegmentState s2(fmin(t3, t4), fmax(t3, t4));

        IntersectionState u1(&d1, &d2);
        IntersectionState u2(&s1, &s2);

        for (int i = 0; i <= 100; i++) {
            if (u1.contains(i) && !(i == t1 && i == t2)) {
                return false;
            }

            if (u2.contains(i) && !((i >= fmin(t1, t2) && i <= fmax(t1, t2)) && (i >= fmin(t3, t4) && i <= fmax(t3, t4)))) {
                return false;
            }
        }

        return true;
    }

    void static test_all() {
        std::cout << "Testing: 1 - OK, 0 - something is wrong" << std::endl << std::endl;
        std::cout << "DiscreteState: " << test_DiscreteState() << std::endl;
        std::cout << "SegmentState: " << test_SegmentState() << std::endl;
        std::cout << "ContGaps: " << test_ContGaps() << std::endl;
        std::cout << "ContAdds: " << test_ContAdds() << std::endl;
        std::cout << "ContGapsAdds: " << test_ContGapsAdds() << std::endl;
        std::cout << "SetState: " << test_SetState() << std::endl;
        std::cout << "UnionState: " << test_UnionState() << std::endl;
        std::cout << "IntersectionState: " << test_IntersectionState() << std::endl;
    }
};

void calc1() {
    SegmentState s(0, 50);

    for (int n = 1; n <= 1000; n++) {
        ProbabilityTest pt(2021, 0, 100, n);
        std::cout << pt(s) << std::endl;
    }
}

void calc2() {
    SegmentState s1(0, 10);
    SegmentState s2(10, 30);

    State* ss1 = Factory::create_UnionState(&s1, &s2);

    for (int n = 1; n <= 1000; n++) {
        ProbabilityTest pt(2021, 0, 60, n);
        std::cout << pt(*ss1) << std::endl;
    }

    Factory::release(ss1);
}

void calc3() {
    SetState s1({ 7, 27, 3, 14, 22, 25, 1, 8, 9, 20, 30, 12, 2, 4, 15, 16, 29, 18, 19, 10, 21, 5, 23, 24, 6, 26, 13, 28, 17, 11 });
    for (int n = 1; n <= 1000; n++) {
        ProbabilityTest pt(2021, 0, 60, n);
        std::cout << pt(s1) << std::endl;
    }
}

int main(int argc, const char* argv[]) { 

    Tester::test_all();

    //calc1();
    //calc2();
    //calc3();

    return 0;
}