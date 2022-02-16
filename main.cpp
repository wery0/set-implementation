#include "Set.h"
#include <random>

int main() {
    std::mt19937 rnd(100);
    std::uniform_int_distribution<int> gen(1, 5);
    Set<std::pair<int, int>> set;
    for (int i = 0; i < 25; ++i) {
        std::pair<int, int> par = {gen(rnd), gen(rnd)};
        set.insert(par);
    }
    std::cout << "Set size = " << set.size() << std::endl;
    for (auto par: set) {
        std::cout << par.first << " " << par.second << std::endl;
    }
}