#include "sc/SCClient.hpp"
#include "spdlog/spdlog.h"

void messageHandler(const std::array<uint8_t, BUFFER_SIZE>& data, size_t size) {
  spdlog::info("Received {} bytes: '{}'", size,
               reinterpret_cast<const char*>(data.data()), size);
}

int main() {
  SCClient client("/tmp/example.sock", spdlog::default_logger());

  if (!client.connectToServer()) {
    spdlog::error("Initial connection failed, client will retry automatically");
  }

  client.registerReceiveCb(messageHandler);

  std::string input;
  while (true) {
    spdlog::info("Enter message: ");

    std::getline(std::cin, input);
    if (input == "exit") break;

    std::vector<uint8_t> data(input.begin(), input.end());
    client.send(data);

    spdlog::info("Bytes sent: {}, total sent: {}", client.getLastBytesSent(),
                 client.getTotalBytesSent());
    spdlog::info("Bytes received: {}, total received: {}",
                 client.getLastBytesReceived(), client.getTotalBytesReceived());
  }
}
