#pragma once

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <array>
#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "spdlog/spdlog.h"

constexpr size_t BUFFER_SIZE = 65 * 1024;
constexpr int MAX_RETRY_DELAY = 32;  // Max delay in seconds for backoff

class SCClient {
 public:
  enum class State { DISCONNECTED, CONNECTING, CONNECTED };

  explicit SCClient(const std::string& socketPath,
                    const std::shared_ptr<spdlog::logger>& logger);

  ~SCClient();

  bool connectToServer();
  void send(const std::vector<uint8_t>& data);

  using ReceiveCbT =
      std::function<void(const std::array<uint8_t, BUFFER_SIZE>&, size_t)>;
  void registerReceiveCb(ReceiveCbT cb);
  State getStatus() const;

  // Byte tracking
  size_t getTotalBytesSent() const;
  size_t getTotalBytesReceived() const;
  size_t getLastBytesSent() const;
  size_t getLastBytesReceived() const;

 private:
  const std::string& socketPath;
  const std::shared_ptr<spdlog::logger> logger;
  int client_fd{-1};
  ReceiveCbT receiveCb;
  std::thread receiveThread;
  std::mutex sendMutex;
  std::atomic<bool> running{false};
  std::atomic<State> connectionState{State::DISCONNECTED};

  std::array<uint8_t, BUFFER_SIZE> recvBuffer;

  // Byte tracking
  std::atomic<size_t> totalBytesSent{0};
  std::atomic<size_t> totalBytesReceived{0};
  std::atomic<size_t> lastBytesSent{0};
  std::atomic<size_t> lastBytesReceived{0};

  void receiveLoop();
  void reconnect();
  void setNonBlocking(int fd);
};
