#include <chrono>
#include <thread>

#include "TestServer.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "sc/SCClient.hpp"
#include "spdlog/spdlog.h"

TEST(sc, EchoTest) {
  const std::string path = "/tmp/sc_test.sock";

  TestServer server(path);
  server.start();

  // Give the server some slack
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(100ms);

  SCClient client(path, spdlog::default_logger());
  ASSERT_TRUE(client.connectToServer());

  const std::string input = "Hello World";
  const std::vector<uint8_t> data(input.begin(), input.end());

  client.registerReceiveCb([input, &server](
                               const std::array<uint8_t, BUFFER_SIZE>& data,
                               size_t size) {
    try {
      std::string received(reinterpret_cast<const char*>(data.data()), size);
      spdlog::info("Received {} bytes: '{}'", size, received);

      ASSERT_EQ(input.c_str(), received);

      server.stop();

    } catch (const std::exception& e) {
      spdlog::error("Exception in callback: {}", e.what());
    }
  });

  client.send(data);
}
