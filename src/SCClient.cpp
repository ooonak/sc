#include "sc/SCClient.hpp"

#include <spdlog/spdlog.h>

#include <cerrno>
#include <thread>

SCClient::SCClient(const std::string& socketPath,
                   const std::shared_ptr<spdlog::logger>& logger)
    : socketPath{socketPath}, logger{logger} {}

SCClient::~SCClient() {
  running = false;
  if (receiveThread.joinable()) {
    receiveThread.join();
  }
  if (client_fd != -1) {
    close(client_fd);
  }
  spdlog::info("Shutting down (~SCClient)");
}

void SCClient::setNonBlocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags != -1) {
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  }
}

bool SCClient::connectToServer() {
  spdlog::info("About to connect to {}", socketPath);

  connectionState = State::CONNECTING;
  client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (client_fd == -1) {
    spdlog::error("Socket creation failed: {}", strerror(errno));
    return false;
  }

  sockaddr_un addr{};
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path) - 1);

  if (connect(client_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    spdlog::error("Connect failed: {}", strerror(errno));
    close(client_fd);
    client_fd = -1;
    connectionState = State::DISCONNECTED;
    return false;
  }

  setNonBlocking(client_fd);
  running = true;
  connectionState = State::CONNECTED;

  spdlog::info("Connected to server.");
  receiveThread = std::thread(&SCClient::receiveLoop, this);

  return true;
}

void SCClient::send(const std::vector<uint8_t>& data) {
  if (getStatus() != State::CONNECTED) {
    spdlog::warn("Attempted to send while disconnected.");
    return;
  }

  std::lock_guard<std::mutex> lock(sendMutex);
  struct iovec iov;
  struct msghdr msg{};

  iov.iov_base = const_cast<uint8_t*>(data.data());
  iov.iov_len = data.size();
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  ssize_t sent = sendmsg(client_fd, &msg, 0);
  if (sent == -1) {
    spdlog::error("Send failed: {}", strerror(errno));
    if (errno == EPIPE || errno == ECONNRESET) {
      running = false;
      connectionState = State::DISCONNECTED;
    }
  } else {
    lastBytesSent = sent;
    totalBytesSent += sent;
    spdlog::info("Sent {} bytes (Total: {}).", sent, totalBytesSent.load());
  }
}

void SCClient::receiveLoop() {
  struct iovec iov;
  struct msghdr msg{};

  iov.iov_base = recvBuffer.data();
  iov.iov_len = recvBuffer.size();
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  while (running) {
    ssize_t bytes_received = recvmsg(client_fd, &msg, 0);
    if (bytes_received > 0) {
      lastBytesReceived = bytes_received;
      totalBytesReceived += bytes_received;
      spdlog::info("Received {} bytes (Total: {}).", bytes_received,
                   totalBytesReceived.load());
      if (receiveCb) {
        receiveCb(recvBuffer, bytes_received);
      }
    }
    else if (bytes_received == 0 ||
               (bytes_received == -1 && errno != EAGAIN)) {
      spdlog::warn("Connection lost, attempting to reconnect...");
      running = false;
      connectionState = State::DISCONNECTED;
      reconnect();
      break;
    }
  }

  close(client_fd);
  client_fd = -1;
}

void SCClient::reconnect() {
  int delay = 1;
  while (!running) {
    std::this_thread::sleep_for(std::chrono::seconds(delay));
    spdlog::warn("Reconnecting attempt...");

    if (connectToServer()) {
      spdlog::info("Reconnected successfully.");
      return;
    }

    delay = std::min(delay * 2, MAX_RETRY_DELAY);
  }
}

SCClient::State SCClient::getStatus() const { return connectionState.load(); }

void SCClient::registerReceiveCb(ReceiveCbT cb) { receiveCb = std::move(cb); }

size_t SCClient::getLastBytesSent() const { return lastBytesSent; }

size_t SCClient::getTotalBytesSent() const { return totalBytesSent; }

size_t SCClient::getLastBytesReceived() const { return lastBytesReceived; }

size_t SCClient::getTotalBytesReceived() const { return totalBytesReceived; }
