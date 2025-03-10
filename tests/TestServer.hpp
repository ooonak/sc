#pragma once

#include <atomic>
#include <thread>

class TestServer {
 public:
  explicit TestServer(const std::string& socketPath);

  ~TestServer();

  void start();

  void stop();

 private:
  void run();

  void handleClient(int client_fd);

  std::string socketPath;
  std::atomic<bool> running{false};
  std::thread serverThread;
  int server_fd{-1};
};
