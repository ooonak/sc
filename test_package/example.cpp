#include "sc/sc.hpp"
#include <vector>
#include <string>

int main() {
    sc();

    std::vector<std::string> vec;
    vec.push_back("test_package");

    sc_print_vector(vec);
}
