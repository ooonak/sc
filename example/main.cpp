#include "sc/sc.hpp"
#include <vector>
#include <string>

int main() {
    sc();

    std::vector<std::string> vec;
    vec.emplace_back("test_package");

    sc_print_vector(vec);
}
