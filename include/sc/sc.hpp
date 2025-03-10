#pragma once

#include <string>
#include <vector>

#ifdef _WIN32
#define SC_EXPORT __declspec(dllexport)
#else
#define SC_EXPORT
#endif

SC_EXPORT void sc();
SC_EXPORT void sc_print_vector(const std::vector<std::string> &strings);
