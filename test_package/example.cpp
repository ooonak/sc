#include "sc/SCClient.hpp"
#include "spdlog/spdlog.h"

int main() { SCClient("/tmp/example.sock", spdlog::default_logger()); }
